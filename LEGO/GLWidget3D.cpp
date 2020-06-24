#include "GLWidget3D.h"
#include "MainWindow.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include <QDir>
#include <QTextStream>
#include <QDate>
#include <iostream>
#include <QProcess>
#include "GLUtils.h"
#include "util/ContourUtils.h"
#include "simp/BuildingSimplification.h"
#include "util/DisjointVoxelData.h"
#include "util/OBJWriter.h"
#include "util/PlyWriter.h"
#include "util/TopFaceWriter.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#define PI 3.14159265

GLWidget3D::GLWidget3D(MainWindow *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers)) {
	this->mainWin = parent;
	ctrlPressed = false;
	shiftPressed = false;

	first_paint = true;

	// This is necessary to prevent the screen overdrawn by OpenGL
	setAutoFillBackground(false);

	// light direction for shadow mapping
	light_dir = glm::normalize(glm::vec3(-1, 1, -3));

	// model/view/projection matrices for shadow mapping
	glm::mat4 light_pMatrix = glm::ortho<float>(-1800, 1800, -1800, 1800, 0.1, 3600);
	glm::mat4 light_mvMatrix = glm::lookAt(-light_dir * 300.0f, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	light_mvpMatrix = light_pMatrix * light_mvMatrix;

	// spot light
	spot_light_pos = glm::vec3(20, 25, 30);

	offset = glm::dvec3(0, 0, 0);
	scale = 0.25;
	min_hole_ratio = 0.02;
	min_height = 1.5;

	color_mode = COLOR;
	show_mode = SHOW_INPUT;
}

/**
* Draw the scene.
*/
void GLWidget3D::drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(true);

	renderManager.renderAll();
}

void GLWidget3D::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// PASS 1: Render to texture
	glUseProgram(renderManager.programs["pass1"]);

	glBindFramebuffer(GL_FRAMEBUFFER, renderManager.fragDataFB);
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderManager.fragDataTex[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, renderManager.fragDataTex[1], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, renderManager.fragDataTex[2], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, renderManager.fragDataTex[3], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderManager.fragDepthTex, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, DrawBuffers); // "3" is the size of DrawBuffers
	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("+ERROR: GL_FRAMEBUFFER_COMPLETE false\n");
		exit(0);
	}

	glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["pass1"], "mvpMatrix"), 1, false, &camera.mvpMatrix[0][0]);
	glUniform3f(glGetUniformLocation(renderManager.programs["pass1"], "lightDir"), light_dir.x, light_dir.y, light_dir.z);
	glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["pass1"], "light_mvpMatrix"), 1, false, &light_mvpMatrix[0][0]);
	glUniform3f(glGetUniformLocation(renderManager.programs["pass1"], "spotLightPos"), spot_light_pos.x, spot_light_pos.y, spot_light_pos.z);
	glUniform3f(glGetUniformLocation(renderManager.programs["pass1"], "cameraPos"), camera.pos.x, camera.pos.y, camera.pos.z);

	glUniform1i(glGetUniformLocation(renderManager.programs["pass1"], "shadowMap"), 6);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, renderManager.shadow.textureDepth);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	drawScene();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// PASS 2: Create AO
	if (renderManager.renderingMode == RenderManager::RENDERING_MODE_SSAO) {
		glUseProgram(renderManager.programs["ssao"]);
		glBindFramebuffer(GL_FRAMEBUFFER, renderManager.fragDataFB_AO);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderManager.fragAOTex, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderManager.fragDepthTex_AO, 0);
		GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Always check that our framebuffer is ok
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			printf("++ERROR: GL_FRAMEBUFFER_COMPLETE false\n");
			exit(0);
		}

		glDisable(GL_DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);

		glUniform2f(glGetUniformLocation(renderManager.programs["ssao"], "pixelSize"), 2.0f / this->width(), 2.0f / this->height());

		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "tex0"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[0]);

		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "tex1"), 2);
		glActiveTexture(GL_TEXTURE2);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[1]);

		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "tex2"), 3);
		glActiveTexture(GL_TEXTURE3);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[2]);

		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "depthTex"), 8);
		glActiveTexture(GL_TEXTURE8);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDepthTex);

		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "noiseTex"), 7);
		glActiveTexture(GL_TEXTURE7);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragNoiseTex);

		{
			glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["ssao"], "mvpMatrix"), 1, false, &camera.mvpMatrix[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["ssao"], "pMatrix"), 1, false, &camera.pMatrix[0][0]);
		}

		glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "uKernelSize"), renderManager.uKernelSize);
		glUniform3fv(glGetUniformLocation(renderManager.programs["ssao"], "uKernelOffsets"), renderManager.uKernelOffsets.size(), (const GLfloat*)renderManager.uKernelOffsets.data());

		glUniform1f(glGetUniformLocation(renderManager.programs["ssao"], "uPower"), renderManager.uPower);
		glUniform1f(glGetUniformLocation(renderManager.programs["ssao"], "uRadius"), renderManager.uRadius);

		glBindVertexArray(renderManager.secondPassVAO);

		glDrawArrays(GL_QUADS, 0, 4);
		glBindVertexArray(0);
		glDepthFunc(GL_LEQUAL);
	}
	else if (renderManager.renderingMode == RenderManager::RENDERING_MODE_LINE || renderManager.renderingMode == RenderManager::RENDERING_MODE_HATCHING) {
		glUseProgram(renderManager.programs["line"]);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);

		glUniform2f(glGetUniformLocation(renderManager.programs["line"], "pixelSize"), 1.0f / this->width(), 1.0f / this->height());
		glUniformMatrix4fv(glGetUniformLocation(renderManager.programs["line"], "pMatrix"), 1, false, &camera.pMatrix[0][0]);
		if (renderManager.renderingMode == RenderManager::RENDERING_MODE_HATCHING) {
			glUniform1i(glGetUniformLocation(renderManager.programs["line"], "useHatching"), 1);
		}
		else {
			glUniform1i(glGetUniformLocation(renderManager.programs["line"], "useHatching"), 0);
		}

		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "tex0"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[0]);

		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "tex1"), 2);
		glActiveTexture(GL_TEXTURE2);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[1]);

		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "tex2"), 3);
		glActiveTexture(GL_TEXTURE3);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[2]);

		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "tex3"), 4);
		glActiveTexture(GL_TEXTURE4);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[3]);

		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "depthTex"), 8);
		glActiveTexture(GL_TEXTURE8);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDepthTex);

		glUniform1i(glGetUniformLocation(renderManager.programs["line"], "hatchingTexture"), 5);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_3D, renderManager.hatchingTextures);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindVertexArray(renderManager.secondPassVAO);

		glDrawArrays(GL_QUADS, 0, 4);
		glBindVertexArray(0);
		glDepthFunc(GL_LEQUAL);
	}
	else if (renderManager.renderingMode == RenderManager::RENDERING_MODE_CONTOUR) {
		glUseProgram(renderManager.programs["contour"]);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);

		glUniform2f(glGetUniformLocation(renderManager.programs["contour"], "pixelSize"), 1.0f / this->width(), 1.0f / this->height());

		glUniform1i(glGetUniformLocation(renderManager.programs["contour"], "depthTex"), 8);
		glActiveTexture(GL_TEXTURE8);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDepthTex);

		glBindVertexArray(renderManager.secondPassVAO);

		glDrawArrays(GL_QUADS, 0, 4);
		glBindVertexArray(0);
		glDepthFunc(GL_LEQUAL);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Blur

	if (renderManager.renderingMode == RenderManager::RENDERING_MODE_BASIC || renderManager.renderingMode == RenderManager::RENDERING_MODE_SSAO) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);

		glUseProgram(renderManager.programs["blur"]);
		glUniform2f(glGetUniformLocation(renderManager.programs["blur"], "pixelSize"), 2.0f / this->width(), 2.0f / this->height());
		//printf("pixelSize loc %d\n", glGetUniformLocation(vboRenderManager.programs["blur"], "pixelSize"));

		glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "tex0"), 1);//COLOR
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[0]);

		glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "tex1"), 2);//NORMAL
		glActiveTexture(GL_TEXTURE2);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[1]);

		/*glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "tex2"), 3);
		glActiveTexture(GL_TEXTURE3);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDataTex[2]);*/

		glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "depthTex"), 8);
		glActiveTexture(GL_TEXTURE8);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragDepthTex);

		glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "tex3"), 4);//AO
		glActiveTexture(GL_TEXTURE4);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, renderManager.fragAOTex);

		if (renderManager.renderingMode == RenderManager::RENDERING_MODE_SSAO) {
			glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "ssao_used"), 1); // ssao used
		}
		else {
			glUniform1i(glGetUniformLocation(renderManager.programs["blur"], "ssao_used"), 0); // no ssao
		}

		glBindVertexArray(renderManager.secondPassVAO);

		glDrawArrays(GL_QUADS, 0, 4);
		glBindVertexArray(0);
		glDepthFunc(GL_LEQUAL);

	}

	// REMOVE
	glActiveTexture(GL_TEXTURE0);
}

void GLWidget3D::loadVoxelData(const QString& filename) {
	// get directory
	QDir dir = QFileInfo(filename).absoluteDir();

	// scan all the files in the directory to get a voxel data
	QStringList files = dir.entryList(QDir::NoDotAndDotDot | QDir::Files, QDir::DirsFirst);
	std::vector<cv::Mat_<uchar>> voxel_data(files.size());
	for (int i = 1; i < files.size(); i++) {
		voxel_data[i - 1] = cv::imread((dir.absolutePath() + "/" + files[i]).toUtf8().constData(), cv::IMREAD_GRAYSCALE);
	}
	vdb_size = cv::Point3i(voxel_data[0].cols, voxel_data[0].rows, voxel_data.size());

	voxel_buildings = util::DisjointVoxelData::disjoint(voxel_data);

	show_mode = SHOW_INPUT;
	update3DGeometry();
}

void GLWidget3D::saveOBJ(const QString& filename) {
	if (show_mode == SHOW_INPUT) {
		util::obj::OBJWriter::writeVoxels(filename.toUtf8().constData(), vdb_size.x, vdb_size.y, offset.x, offset.y, offset.z, scale, voxel_buildings);
	}
	else {
		util::obj::OBJWriter::write(filename.toUtf8().constData(), vdb_size.x, vdb_size.y, offset.x, offset.y, offset.z, scale, buildings);
	}
}

void GLWidget3D::saveXYZN(const QString& filename) {
	if (show_mode == SHOW_INPUT) {
		util::obj::OBJWriter::writePointCloud_XYZN(filename.toUtf8().constData(), vdb_size.x, vdb_size.y, offset.x, offset.y, offset.z, scale, voxel_buildings);
	}
	else {
		util::obj::OBJWriter::write(filename.toUtf8().constData(), vdb_size.x, vdb_size.y, offset.x, offset.y, offset.z, scale, buildings);
	}
}

void GLWidget3D::saveTopFace(const QString& filename) {
	util::topface::TopFaceWriter::write(filename.toUtf8().constData(), vdb_size.x, vdb_size.y, offset.x, offset.y, offset.z, scale, buildings);
}

void GLWidget3D::savePLY(const QString& filename) {
	util::ply::PlyWriter::write(filename.toUtf8().constData(), vdb_size.x, vdb_size.y, offset.x, offset.y, offset.z, scale, buildings);
}

void GLWidget3D::saveImage(const QString& filename) {
	QImage image = grabFrameBuffer();
	image.save(filename);
}

void GLWidget3D::showInputVoxel() {
	show_mode = SHOW_INPUT;
	update3DGeometry(voxel_buildings);
}

void GLWidget3D::simplifyByAll(double alpha) {
	// determine the layering threshold based on the weight ratio
	float threshold = 0.01;
	if (alpha < 0.1) threshold = 0.1;
	else if (alpha < 0.2) threshold = 0.3;
	else if (alpha < 0.3) threshold = 0.4;
	else if (alpha < 0.4) threshold = 0.5;
	else if (alpha < 0.5) threshold = 0.6;
	else if (alpha < 0.7) threshold = 0.6;
	else if (alpha < 0.9) threshold = 0.7;
	else threshold = 0.99;

	buildings = simp::BuildingSimplification::simplifyBuildings(voxel_buildings, simp::BuildingSimplification::ALG_ALL, false, min_height / scale, alpha, threshold, 0, 0, 0, 0, min_hole_ratio);

	show_mode = SHOW_ALL;
	update3DGeometry();
}

/**
* Simplify all the buildings by curve++ method.
*
* @param epsilon				The epsilon for DP method
* @param layering_threshold		layering threshold
* @param snapping_threshold		Threshold for snapping
* @param orientation			principle orientation of the building in radian
* @param min_contour_area		Minimum area of the contour [m^2].
* @param allow_triangle_contour	True if a triangle is allowed as a simplified contour shape
*/
void GLWidget3D::simplifyByDP(double epsilon, double layering_threshold, double snapping_threshold, double orientation, double min_contour_area, float max_obb_ratio, bool allow_triangle_contour, bool allow_overhang) {
	std::map<int, std::vector<double>> algorithms;
	algorithms[simp::BuildingSimplification::ALG_DP] = { epsilon };
	buildings = simp::BuildingSimplification::simplifyBuildings(voxel_buildings, algorithms, false, min_height / scale, 0.5, layering_threshold, snapping_threshold / scale, orientation, min_contour_area / scale / scale, max_obb_ratio, allow_triangle_contour, allow_overhang, min_hole_ratio);

	show_mode = SHOW_DP;
	update3DGeometry();
}

/**
* Simplify all the buildings by curve++ method.
*
* @param epsilon				The resolution
* @param layering_threshold		layering threshold
* @param snapping_threshold		Threshold for snapping
* @param orientation			principle orientation of the building in radian
* @param min_contour_area		Minimum area of the contour [m^2].
* @param allow_triangle_contour	True if a triangle is allowed as a simplified contour shape
*/
void GLWidget3D::simplifyByRightAngle(int resolution, bool optimization, double layering_threshold, double snapping_threshold, double orientation, double min_contour_area, float max_obb_ratio, bool allow_triangle_contour, bool allow_overhang) {
	std::map<int, std::vector<double>> algorithms;
	algorithms[simp::BuildingSimplification::ALG_RIGHTANGLE] = { (double)resolution, optimization ? 1.0 : 0.0 };
	buildings = simp::BuildingSimplification::simplifyBuildings(voxel_buildings, algorithms, false, min_height / scale, 0.5, layering_threshold, snapping_threshold / scale, orientation, min_contour_area / scale / scale, max_obb_ratio, allow_triangle_contour, allow_overhang, min_hole_ratio);

	show_mode = SHOW_RIGHTANGLE;
	update3DGeometry();
}

/**
* Simplify all the buildings by curve++ method.
*
* @param epsilon				The epsilon for DP method
* @param curve_threshold		curve threshold
* @param layering_threshold		layering threshold
* @param snapping_threshold		Threshold for snapping
* @param orientation			principle orientation of the building in radian
* @param min_contour_area		Minimum area of the contour [m^2].
* @param allow_triangle_contour	True if a triangle is allowed as a simplified contour shape
*/
void GLWidget3D::simplifyByCurve(double epsilon, double curve_threshold, double layering_threshold, double snapping_threshold, double orientation, double min_contour_area, float max_obb_ratio, bool allow_triangle_contour, bool allow_overhang) {
	std::map<int, std::vector<double>> algorithms;
	algorithms[simp::BuildingSimplification::ALG_CURVE] = { epsilon, curve_threshold };
	buildings = simp::BuildingSimplification::simplifyBuildings(voxel_buildings, algorithms, false, min_height / scale, 0.5, layering_threshold, snapping_threshold / scale, orientation, min_contour_area / scale / scale, max_obb_ratio, allow_triangle_contour, allow_overhang, min_hole_ratio);

	show_mode = SHOW_CURVE;
	update3DGeometry();
}

/**
 * Simplify all the buildings by curve++ method.
 * 
 * @param epsilon				The epsilon for DP method
 * @param curve_threshold		curve threshold
 * @param angle_threshold		angle threshold in radian
 * @param layering_threshold	layering threshold
 * @param snapping_threshold	Threshold for snapping
 * @param orientation			principle orientation of the building in radian
 * @param min_contour_area		Minimum area of the contour [m^2].
 * @param allow_triangle_contour	True if a triangle is allowed as a simplified contour shape
 */
void GLWidget3D::simplifyByCurveRightAngle(double epsilon, double curve_threshold, double angle_threshold, double layering_threshold, double snapping_threshold, double orientation, double min_contour_area, float max_obb_ratio, bool allow_triangle_contour, bool allow_overhang) {
	std::map<int, std::vector<double>> algorithms;
	algorithms[simp::BuildingSimplification::ALG_CURVE_RIGHTANGLE] = { epsilon, curve_threshold, angle_threshold };
	buildings = simp::BuildingSimplification::simplifyBuildings(voxel_buildings, algorithms, false, min_height / scale, 0.5, layering_threshold, snapping_threshold / scale, orientation, min_contour_area / scale / scale, max_obb_ratio, allow_triangle_contour, allow_overhang, min_hole_ratio);

	show_mode = SHOW_CURVE;
	update3DGeometry();
}

/**
* Simplify all the buildings by efficientRansac method.
*
* @param curve_num_iterations				The iterations for curve detection
* @param curve_min_points					minimum number of points on the curve
* @param curve_max_error_ratio_to_radius	max error for supporting points on the curve
* @param curve_cluster_epsilon				the cluster to choose curve
* @param curve_min_angle					minimum angle for the curve
* @param curve_min_radius					minimum radius for the curve
* @param curve_max_radius					maximum radius for the curve
* @param line_num_iterations				The iterations for line detection
* @param line_min_points					minimum number of points on the line segment
* @param line_max_error						max error for supporting points on the line
* @param line_cluster_epsilon				the cluster to choose line
* @param line_min_length					minimum length for the line
* @param line_angle_threshold				angle threshold for the line
* @param allow_triangle_contour	True if a triangle is allowed as a simplified contour shape
*/
void GLWidget3D::simplifyByEfficientRansac(double curve_num_iterations, double curve_min_points, double curve_max_error_ratio_to_radius, double curve_cluster_epsilon, double curve_min_angle, double curve_min_radius, double curve_max_radius, double line_num_iterations, double line_min_points, double line_max_error, double line_cluster_epsilon, double line_min_length, double line_angle_threshold, double contour_max_error, double contour_angle_threshold, double layering_threshold, double snapping_threshold, double orientation, double min_contour_area, float max_obb_ratio, bool allow_triangle_contour, bool allow_overhang, QString config_filename){
	std::map<int, std::vector<double>> algorithms;
	std::vector<regularizer::Config> regularizer_configs;
	algorithms[simp::BuildingSimplification::ALG_EFFICIENT_RANSAC] = { curve_num_iterations, curve_min_points, curve_max_error_ratio_to_radius, curve_cluster_epsilon, curve_min_angle, curve_min_radius, curve_max_radius, line_num_iterations, line_min_points, line_max_error, line_cluster_epsilon, line_min_length, line_angle_threshold, contour_max_error, contour_angle_threshold };
	if (!config_filename.isEmpty()){
		QFile file(config_filename);
		if (!file.open(QIODevice::ReadOnly)) {
			std::cerr << "File was not readable: " << std::endl;
		}
		else{
			QTextStream in(&file);
			rapidjson::Document doc;
			doc.Parse(in.readAll().toUtf8().constData());
			int num_runs = doc["num_runs"].GetInt();
			std::cout << "num_runs is " << num_runs << std::endl;
			regularizer_configs.resize(num_runs);
			for (int i = 0; i < num_runs; i++){
				bool bUseIntra = false;
				float intraWeight = 0.0f;
				bool bUseInter = false;
				float interWeight = 0.0f;
				bool bUseRaOpt = false;
				float angle_threshold_RA = 0.0f;
				float raWeight = 0.0f;
				bool bUseParallelOpt = false;
				float angle_threshold_parallel = 0.0f;
				float parallelWeight = 0.0f;
				bool bUseSymmetryLineOpt = false;
				float symmetryIouThreshold = 0.0f;
				float symmetryWeight = 0.0f;
				bool bUseAccuracyOpt = false;
				float accuracyWeight = 0.0f;
				bool bUsePointSnapOpt = false;
				float pointDisThreshold = 0.0f;
				float pointWeight = 0.0f;
				bool bUseSegSnapOpt = false;
				float segDisThreshold = 0.0f;
				float segAngleThreshold = 0.0f;
				float segWeight = 0.0f;

				// get parameters for each run
				QString run_name = "Opt" + QString::number(i + 1);
				rapidjson::Value& algs_run = doc[run_name.toUtf8().constData()];
				
				bUseIntra = algs_run["UseIntra"].GetBool();
				bUseInter = algs_run["UseInter"].GetBool();
				intraWeight = algs_run["Intra_Weight"].GetFloat();
				interWeight = algs_run["Inter_Weight"].GetFloat();
				if (bUseIntra){
					rapidjson::Value& algs = algs_run["IntraOpt"];
					//ra
					rapidjson::Value& algs_ra = algs["RA"];
					bUseRaOpt = algs_ra["UseOpt"].GetBool();
					angle_threshold_RA = algs_ra["AngleThreshold"].GetFloat();
					raWeight = algs_ra["Weight"].GetFloat();
					//symmetry
					rapidjson::Value& algs_symmetry = algs["Symmetry"]; 
					bUseSymmetryLineOpt = algs_symmetry["UseOpt"].GetBool();
					symmetryIouThreshold = algs_symmetry["IouThreshold"].GetFloat();
					symmetryWeight = algs_symmetry["Weight"].GetFloat();
					// parallel
					rapidjson::Value& algs_parallel = algs["Parallel"];
					bUseParallelOpt = algs_parallel["UseOpt"].GetBool();
					angle_threshold_parallel = algs_parallel["AngleThreshold"].GetFloat();
					parallelWeight = algs_parallel["Weight"].GetFloat();
					// accuracy
					rapidjson::Value& algs_accuracy = algs["Accuracy"];
					bUseAccuracyOpt = algs_accuracy["UseOpt"].GetBool();
					accuracyWeight = algs_accuracy["Weight"].GetFloat();
				}
				if (bUseInter){
					rapidjson::Value& algs = algs_run["InterOpt"];
					// point snap
					rapidjson::Value& algs_point = algs["PointSnap"];
					bUsePointSnapOpt = algs_point["UseOpt"].GetBool();
					pointDisThreshold = algs_point["DisThreshold"].GetFloat();
					pointWeight = algs_point["Weight"].GetFloat();
					// seg snap
					rapidjson::Value& algs_seg = algs["SegSnap"];
					bUseSegSnapOpt = algs_seg["UseOpt"].GetBool();
					segDisThreshold = algs_seg["DisThreshold"].GetFloat();
					segAngleThreshold = algs_seg["AngleThreshold"].GetFloat();
					segWeight = algs_seg["Weight"].GetFloat();
				}
				//check weights
				if (bUseInter || bUseIntra){
					float weight = 0.0f;
					if (bUseInter)
						weight += interWeight;
					if (bUseIntra)
						weight += intraWeight;
					if (abs(weight - 1.0f) < 0.0001)
					{
						//
					}
					else{
						std::cout << "Please check intra and inter weight assignment!!!" << std::endl;
						return;
					}
				}
				if (bUseIntra){
					//check the sum weight equals 1
					float weight = 0.0f;
					if (bUseRaOpt)
						weight += raWeight;
					if (bUseParallelOpt)
						weight += parallelWeight;
					if (bUseSymmetryLineOpt)
						weight += symmetryWeight;
					if (bUseAccuracyOpt)
						weight += accuracyWeight;
					if (abs(weight - 1.0f) < 0.0001)
					{
						//
					}
					else{
						std::cout << "Please check intra weight assignment!!!" << std::endl;
						return;
					}
				}
				if (bUseInter){
					float weight = 0.0f;
					if (bUsePointSnapOpt)
						weight += pointWeight;
					if (bUseSegSnapOpt)
						weight += segWeight;
					if (abs(weight - 1.0f) < 0.0001)
					{
						//
					}
					else{
						std::cout << "Please check inter weight assignment!!!" << std::endl;
						return;
					}
				}
				regularizer::Config config(bUseIntra, intraWeight, bUseInter, interWeight, bUseRaOpt, angle_threshold_RA, raWeight, bUseParallelOpt, angle_threshold_parallel, parallelWeight, bUseSymmetryLineOpt, symmetryIouThreshold, symmetryWeight, bUseAccuracyOpt, accuracyWeight, bUsePointSnapOpt, pointDisThreshold, pointWeight, bUseSegSnapOpt, segDisThreshold, segAngleThreshold, segWeight);
				regularizer_configs[i] = config;
				/*{
					std::cout << "bUseRa " << config.bUseRaOpt << " ra angle is " << config.angle_threshold_RA << " ra weight is " << config.raWeight << std::endl;
					std::cout << "bUseParallel " << config.bUseParallelOpt << " Parallel angle is " << config.angle_threshold_parallel << " Parallel weight is " << config.parallelWeight << std::endl;
					std::cout << "bUseSymmetry " << config.bUseSymmetryLineOpt << " Symmetry weight is " << config.symmetryWeight << std::endl;
					std::cout << "bUseAccuracy " << config.bUseAccuracyOpt << " Accuracy weight is " << config.accuracyWeight << std::endl;
				}
				{
					std::cout << "bUsePoint " << config.bUsePointSnapOpt << " Point threshold is " << config.pointDisThreshold << " Point weight is " << config.pointWeight << std::endl;
					std::cout << "bUseSeg " << config.bUseSegSnapOpt << " seg angle is " << config.segAngleThreshold << " seg weight is " << config.segWeight << std::endl;
				}*/
			}
			file.close();
		}
	}
	buildings = simp::BuildingSimplification::simplifyBuildings(voxel_buildings, algorithms, false, min_height / scale, 0.5, layering_threshold, snapping_threshold / scale, orientation, min_contour_area / scale / scale, max_obb_ratio, allow_triangle_contour, allow_overhang, min_hole_ratio, regularizer_configs);

	show_mode = SHOW_EFFICIENT_RANSAC;
	update3DGeometry();
}

void GLWidget3D::generateFacadeImages(QString facadeImagesPath, int imageNum, bool bDataAugmentaion, int width, int height, std::pair<int, int> imageRows, std::pair<int, int> imageCols, std::pair<int, int> imageGroups, std::pair<double, double> imageRelativeWidth, std::pair<double, double> imageRelativeHeight, bool bWindowDis, double windowDisRatio, bool bWindowProb, double windowProb, bool bPadding, std::pair<int, int> imagePadding){
	/*
	std::cout << "facadeImagesPath is " << facadeImagesPath.toUtf8().constData() << std::endl;
	std::cout << "imageNum is " << imageNum << std::endl;
	std::cout <<"bDataAugmentaion is " << bDataAugmentaion << std::endl;
	std::cout << "width is " << width << std::endl;
	std::cout << "height is " << height << std::endl;
	std::cout << "imageRows is " << "(" << imageRows.first << ", " << imageRows.second<<")"<< std::endl;
	std::cout << "imageCols is " << "(" << imageCols.first << ", " << imageCols.second << ")" << std::endl;
	std::cout << "imageGroups is " << "(" << imageGroups.first << ", " << imageGroups.second << ")" << std::endl;
	std::cout << "imageRelativeWidth is " << "(" << imageRelativeWidth.first << ", " << imageRelativeWidth.second << ")" << std::endl;
	std::cout << "imageRelativeHeight is " << "(" << imageRelativeHeight.first << ", " << imageRelativeHeight.second << ")" << std::endl;
	std::cout << "bWindowDis is " << bWindowDis << std::endl;
	std::cout << "windowDisRatio is " << windowDisRatio << std::endl;
	std::cout << "bWindowProb is " << bWindowProb << std::endl;
	std::cout << "windowProb is " << windowProb << std::endl;
	std::cout << "bPadding is " << bPadding << std::endl;
	std::cout << "imagePadding is " << "(" << imagePadding.first << ", " << imagePadding.second << ")" << std::endl;
	std::cout << "bDoors is " << bDoors << std::endl;
	std::cout << "imageDoors is " << "(" << imageDoors.first << ", " << imageDoors.second << ")" << std::endl;
	std::cout << "imageDRelativeWidth is " << "(" << imageDRelativeWidth.first << ", " << imageDRelativeWidth.second << ")" << std::endl;
	std::cout << "imageDRelativeHeight is " << "(" << imageDRelativeHeight.first << ", " << imageDRelativeHeight.second << ")" << std::endl;
	*/
	// generate facade images
	int index = 0;
	std::ofstream out_param(facadeImagesPath.toUtf8() + "/parameters.txt", std::ios::app);
	for (int l = 0; l < imageNum; l++){
		cv::Scalar bg_color(255, 255, 255); // white back ground
		cv::Scalar window_color(0, 0, 0); // black for windows
		int NR = util::genRand(imageRows.first, imageRows.second + 1);
		int NG = util::genRand(imageGroups.first, imageGroups.second + 1);
		int NC = 0;
		if (NG == 1)
			NC = util::genRand(imageCols.first, imageCols.second + 1);
		else{
			NC = util::genRand(imageCols.first + 1, imageCols.second / NG + 2);
		}
		double ratioWidth = 0;
		double ratioHeight = 0;
		if (NC == 1){
			ratioWidth = util::genRand(0.9, 1.0);
		}
		else if (NC <= 3){
			ratioWidth = util::genRand(imageRelativeWidth.second - 0.4, imageRelativeWidth.second);
		}
		else{
			ratioWidth = util::genRand(imageRelativeWidth.first, imageRelativeWidth.second);
		}
		ratioHeight = util::genRand(imageRelativeHeight.first, imageRelativeHeight.second);
		int thickness = -1;
		double FH = height * 1.0 / NR;
		double FW = width * 1.0 / NC;
		double WH = FH * ratioHeight;
		double WW = FW * ratioWidth;
		std::cout << "NR is " << NR << std::endl;
		std::cout << "NC is " << NC << std::endl;
		std::cout << "FH is " << FH << std::endl;
		std::cout << "FW is " << FW << std::endl;
		std::cout << "NG is " << NG << std::endl;
		std::cout << "ratioWidth is " << ratioWidth << std::endl;
		std::cout << "ratioHeight is " << ratioHeight << std::endl;
		std::cout << "WH is " << WH << std::endl;
		std::cout << "WW is " << WW << std::endl;
		// draw facade image
		for (int iter_outers = 0; iter_outers < 4; ++iter_outers){
			cv::Mat result(height, width, CV_8UC3, bg_color);
			if (NG == 1){
				for (int i = 0; i < NR; ++i) {
					for (int j = 0; j < NC; ++j) {
						float x1 = (FW - WW) * 0.5 + FW * j;
						float y1 = (FH - WH) * 0.5 + FH * i;
						float x2 = x1 + WW;
						float y2 = y1 + WH;
						//cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), window_color, thickness);
						if (bWindowDis) {
							x1 += util::genRand(-WW * windowDisRatio, WW * windowDisRatio);
							y1 += util::genRand(-WH * windowDisRatio, WH * windowDisRatio);
							x2 += util::genRand(-WW * windowDisRatio, WW * windowDisRatio);
							y2 += util::genRand(-WH * windowDisRatio, WH * windowDisRatio);
						}

						if (bWindowProb){
							if (util::genRand() < windowProb) {
								cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), window_color, thickness);
							}
						}
						else{
							cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), window_color, thickness);
						}
					}
				}
			}
			else{
				double gap = 0.05 * WW; // Assume not too many windows in one group
				double GWW = (WW - gap * (NG - 1)) / NG;
				double GFW = GWW + gap;
				for (int i = 0; i < NR; ++i) {
					for (int j = 0; j < NC; ++j) {
						float x1 = (FW - WW) * 0.5 + FW * j;
						float y1 = (FH - WH) * 0.5 + FH * i;
						for (int k = 0; k < NG; k++){
							float g_x1 = x1 + GFW * k;
							float g_y1 = y1;
							float g_x2 = g_x1 + GWW;
							float g_y2 = g_y1 + WH;

							if (bWindowDis) {
								g_x1 += util::genRand(-GWW * windowDisRatio, GWW * windowDisRatio);
								g_y1 += util::genRand(-WH * windowDisRatio, WH * windowDisRatio);
								g_x2 += util::genRand(-GWW * windowDisRatio, GWW * windowDisRatio);
								g_y2 += util::genRand(-WH * windowDisRatio, WH * windowDisRatio);
							}

							if (bWindowProb){
								if (util::genRand() < windowProb) {
									cv::rectangle(result, cv::Point(std::round(g_x1), std::round(g_y1)), cv::Point(std::round(g_x2), std::round(g_y2)), window_color, thickness);
								}
							}
							else{
								cv::rectangle(result, cv::Point(std::round(g_x1), std::round(g_y1)), cv::Point(std::round(g_x2), std::round(g_y2)), window_color, thickness);
							}
						}
					}
				}
			}

			QString img_filename = facadeImagesPath + QString("/facade_image_%1.png").arg(index, 6, 10, QChar('0'));
			QString img_name = QString("facade_image_%1.png").arg(index, 6, 10, QChar('0'));
			
			// add padding
			if (bPadding){
				int top = util::genRand(imagePadding.first, imagePadding.second + 1);
				int bottom = util::genRand(imagePadding.first, imagePadding.second + 1);
				int left = util::genRand(imagePadding.first, imagePadding.second + 1);
				int right = util::genRand(imagePadding.first, imagePadding.second + 1);
				/*std::cout << "top is " << top << std::endl;
				std::cout << "bottom is " << bottom << std::endl;
				std::cout << "left is " << left << std::endl;
				std::cout << "right is " << right << std::endl;*/
				int borderType = cv::BORDER_CONSTANT;
				cv::copyMakeBorder(result, result, top, bottom, left, right, borderType, bg_color);
				// crop
				{
					int final_width = width + 2 * imagePadding.first;
					int final_height = height + 2 * imagePadding.first;
					int x1, y1;
					x1 = rand() % (int)(result.cols - final_width + 1);
					y1 = rand() % (int)(result.rows - final_height + 1);
					/*std::cout << "x1 is " << x1 << std::endl;
					std::cout << "y1 is " << y1 << std::endl;*/
					result = cv::Mat(result, cv::Rect(x1, y1, final_width, final_height)).clone();
				}
			}
			// add rotation
			if (false)
			{
				cv::cvtColor(result, result, cv::COLOR_BGR2GRAY);
				// rotate the image
				cv::Mat rot_img;
				cv::Point2f offset(result.cols / 2 + rand() % 30 - 15, result.rows / 2 + rand() % 30 - 15);
				float angle = rand() % 5 - 2;
				cv::Mat rot_mat = cv::getRotationMatrix2D(offset, angle, 1.0);
				cv::warpAffine(result, rot_img, rot_mat, result.size(), cv::INTER_CUBIC, cv::BORDER_REPLICATE);
				cv::cvtColor(rot_img, result, cv::COLOR_GRAY2BGR);
			}
			cv::imwrite(img_filename.toUtf8().constData(), result);
			index++;
			// write to parameters.txt
			{
				// normalize for NN training
				out_param << img_name.toUtf8().constData();
				out_param << ",";
				out_param << (NR - imageRows.first) * 1.0 / (imageRows.second - imageRows.first);
				out_param << ",";
				out_param << (NC - imageCols.first) * 1.0 / (imageCols.second - imageCols.first);
				out_param << ",";
				out_param << (ratioWidth - imageRelativeWidth.first) * 1.0 / (imageRelativeWidth.second - imageRelativeWidth.first);
				out_param << ",";
				out_param << (ratioHeight - imageRelativeHeight.first) * 1.0 / (imageRelativeHeight.second - imageRelativeHeight.first);
				out_param << "\n";
			}
			//{
			//	out_param << img_name.toUtf8().constData();
			//	out_param << ",";
			//	if (NR > 1 && NC > 1){
			//		out_param << 1;
			//		out_param << ",";
			//		out_param << 0;
			//		out_param << ",";
			//		out_param << 0;
			//		out_param << ",";
			//		out_param << 0;
			//		out_param << ",";
			//		out_param << 0;
			//		out_param << ",";
			//		out_param << 0;
			//		out_param << "\n";
			//	}
			//	else if (NR == 1){
			//		out_param << 0;
			//		out_param << ",";
			//		out_param << 0;
			//		out_param << ",";
			//		out_param << 1;
			//		out_param << ",";
			//		out_param << 0;
			//		out_param << ",";
			//		out_param << 0;
			//		out_param << ",";
			//		out_param << 0;
			//		out_param << "\n";
			//	}
			//	else if (NC == 1){
			//		out_param << 0;
			//		out_param << ",";
			//		out_param << 0;
			//		out_param << ",";
			//		out_param << 0;
			//		out_param << ",";
			//		out_param << 0;
			//		out_param << ",";
			//		out_param << 1;
			//		out_param << ",";
			//		out_param << 0;
			//		out_param << "\n";
			//	}
			//}
		}
	}
}

void GLWidget3D::generateFacadeImages(QString facadeImagesPath, bool bDataAugmentaion, int width, int height, std::pair<int, int> imageRows, std::pair<int, int> imageCols, std::pair<int, int> imageGroups, std::pair<double, double> imageRelativeWidth, std::pair<double, double> imageRelativeHeight, bool bWindowDis, double windowDisRatio, bool bWindowProb, double windowProb, bool bPadding, std::pair<int, int> imagePadding){
	/*
	std::cout << "facadeImagesPath is " << facadeImagesPath.toUtf8().constData() << std::endl;
	std::cout << "imageNum is " << imageNum << std::endl;
	std::cout <<"bDataAugmentaion is " << bDataAugmentaion << std::endl;
	std::cout << "width is " << width << std::endl;
	std::cout << "height is " << height << std::endl;
	std::cout << "imageRows is " << "(" << imageRows.first << ", " << imageRows.second<<")"<< std::endl;
	std::cout << "imageCols is " << "(" << imageCols.first << ", " << imageCols.second << ")" << std::endl;
	std::cout << "imageGroups is " << "(" << imageGroups.first << ", " << imageGroups.second << ")" << std::endl;
	std::cout << "imageRelativeWidth is " << "(" << imageRelativeWidth.first << ", " << imageRelativeWidth.second << ")" << std::endl;
	std::cout << "imageRelativeHeight is " << "(" << imageRelativeHeight.first << ", " << imageRelativeHeight.second << ")" << std::endl;
	std::cout << "bWindowDis is " << bWindowDis << std::endl;
	std::cout << "windowDisRatio is " << windowDisRatio << std::endl;
	std::cout << "bWindowProb is " << bWindowProb << std::endl;
	std::cout << "windowProb is " << windowProb << std::endl;
	std::cout << "bPadding is " << bPadding << std::endl;
	std::cout << "imagePadding is " << "(" << imagePadding.first << ", " << imagePadding.second << ")" << std::endl;
	std::cout << "bDoors is " << bDoors << std::endl;
	std::cout << "imageDoors is " << "(" << imageDoors.first << ", " << imageDoors.second << ")" << std::endl;
	std::cout << "imageDRelativeWidth is " << "(" << imageDRelativeWidth.first << ", " << imageDRelativeWidth.second << ")" << std::endl;
	std::cout << "imageDRelativeHeight is " << "(" << imageDRelativeHeight.first << ", " << imageDRelativeHeight.second << ")" << std::endl;
	*/
	// generate facade images
	int index = 24;
	double step_W = 0.1;
	double step_H = 0.1;
	int num_W = 0;
	int num_H = 0;
	if (ceil((imageRelativeWidth.second - imageRelativeWidth.first) / step_W) - (imageRelativeWidth.second - imageRelativeWidth.first) / step_W < 0.01)
		num_W = ceil((imageRelativeWidth.second - imageRelativeWidth.first) / step_W);
	else
		num_W = floor((imageRelativeWidth.second - imageRelativeWidth.first) / step_W);

	if (ceil((imageRelativeHeight.second - imageRelativeHeight.first) / step_H) - (imageRelativeHeight.second - imageRelativeHeight.first) / step_H < 0.01)
		num_H = ceil((imageRelativeHeight.second - imageRelativeHeight.first) / step_H);
	else
		num_H = floor((imageRelativeHeight.second - imageRelativeHeight.first) / step_H);
	std::cout << "num_W is " << num_W << std::endl;
	std::cout << "num_H is " << num_H << std::endl;
	cv::Scalar bg_color(0, 0, 0); // white back ground
	cv::Scalar window_color(255, 255, 255); // black for windows
	int thickness = -1;
	std::ofstream out_param(facadeImagesPath.toUtf8() + "/parameters.txt", std::ios::app);
	for (int row = imageRows.first; row <= imageRows.second; row++){ // loop row
		for (int col = imageCols.first; col <= imageCols.second; col++){ // loop col
			for (int relativeW = 0; relativeW <= num_W; relativeW++){ // loop relativeWidth
				for (int relativeH = 0; relativeH <= num_H; relativeH++){
					int NR = row;
					int NG = 1;
					int NC = col;
					double ratioWidth = relativeW * step_W + imageRelativeWidth.first;
					double ratioHeight = relativeH * step_H + imageRelativeHeight.first;
					int thickness = -1;
					double FH = height * 1.0 / NR;
					double FW = width * 1.0 / NC;
					double WH = FH * ratioHeight;
					double WW = FW * ratioWidth;
					if (NC > 1)
						FW = WW + (width - WW * NC) / (NC - 1);
					if (NR > 1)
						FH = WH + (height - WH * NR) / (NR - 1);
					std::cout << "NR is " << NR << std::endl;
					std::cout << "NC is " << NC << std::endl;
					std::cout << "FH is " << FH << std::endl;
					std::cout << "FW is " << FW << std::endl;
					std::cout << "NG is " << NG << std::endl;
					std::cout << "ratioWidth is " << ratioWidth << std::endl;
					std::cout << "ratioHeight is " << ratioHeight << std::endl;
					std::cout << "WH is " << WH << std::endl;
					std::cout << "WW is " << WW << std::endl;

					int num_iters = 10;
					if (row >= 6 && col >= 6)
						num_iters = 30;
					// draw facade image
					for (int iter_outers = 0; iter_outers < 1; ++iter_outers){
						cv::Mat result(height, width, CV_8UC1, bg_color);
						//cv::Mat result_G(height, width, CV_8UC1, bg_color);
						if (NG == 1){
							for (int i = 0; i < NR; ++i) {
								for (int j = 0; j < NC; ++j) {
									float x1 = FW * j;
									float y1 = FH * i;
									float x2 = x1 + WW;
									float y2 = y1 + WH;

									/*float x1_G = x1;
									float y1_G = y1;
									float x2_G = x2;
									float y2_G = y2;*/
									//cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), window_color, thickness);
									if (bWindowDis) {
										x1 += util::genRand(-WW * windowDisRatio, WW * windowDisRatio);
										y1 += util::genRand(-WH * windowDisRatio, WH * windowDisRatio);
										x2 += util::genRand(-WW * windowDisRatio, WW * windowDisRatio);
										y2 += util::genRand(-WH * windowDisRatio, WH * windowDisRatio);
									}

									if (bWindowProb){
										if (util::genRand() < windowProb) {
											cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), window_color, thickness);
											//cv::rectangle(result_G, cv::Point(std::round(x1_G), std::round(y1_G)), cv::Point(std::round(x2_G), std::round(y2_G)), window_color, thickness);
										}
									}
									else{
										cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), window_color, thickness);
										//cv::rectangle(result_G, cv::Point(std::round(x1_G), std::round(y1_G)), cv::Point(std::round(x2_G), std::round(y2_G)), window_color, thickness);
									}
								}
							}
						}

						QString img_filename = facadeImagesPath + QString("/facade_%1.png").arg(index, 5, 10, QChar('0'));
						//QString img_filename_G = facadeImagesPath + QString("/G/facade_image_%1.png").arg(index, 6, 10, QChar('0'));
						QString img_name = QString("facade_%1.png").arg(index, 5, 10, QChar('0'));
						//std::cout << "img_filename_G is " << img_filename_G.toUtf8().constData() << std::endl;
						// add padding
						if (bPadding){
							int top = util::genRand(imagePadding.first, imagePadding.second + 1);
							int bottom = util::genRand(imagePadding.first, imagePadding.second + 1);
							int left = util::genRand(imagePadding.first, imagePadding.second + 1);
							int right = util::genRand(imagePadding.first, imagePadding.second + 1);
							/*std::cout << "top is " << top << std::endl;
							std::cout << "bottom is " << bottom << std::endl;
							std::cout << "left is " << left << std::endl;
							std::cout << "right is " << right << std::endl;*/
							int borderType = cv::BORDER_CONSTANT;
							cv::copyMakeBorder(result, result, top, bottom, left, right, borderType, bg_color);
							//cv::copyMakeBorder(result_G, result_G, top, bottom, left, right, borderType, bg_color);
						}
						cv::imwrite(img_filename.toUtf8().constData(), result);
						//cv::imwrite(img_filename_G.toUtf8().constData(), result_G);
						index++;
						// write to parameters.txt
						{
							// normalize for NN training
							//out_param << img_name.toUtf8().constData();
							out_param << QString("facade_image_%1_fake.png").arg(index - 1, 6, 10, QChar('0')).toUtf8().constData();
							out_param << ",";
							out_param << (NR - imageRows.first) * 1.0 / (imageRows.second - imageRows.first);
							out_param << ",";
							out_param << (NC - imageCols.first) * 1.0 / (imageCols.second - imageCols.first);
							out_param << ",";
							out_param << (ratioWidth - imageRelativeWidth.first) * 1.0 / (imageRelativeWidth.second - imageRelativeWidth.first);
							out_param << ",";
							out_param << (ratioHeight - imageRelativeHeight.first) * 1.0 / (imageRelativeHeight.second - imageRelativeHeight.first);
							out_param << "\n";
						}

						/*{
							out_param << img_name.toUtf8().constData();
							out_param << ",";
							if (NR > 1 && NC > 1){
								out_param << 1;
								out_param << ",";
								out_param << 0;
								out_param << ",";
								out_param << 0;
								out_param << ",";
								out_param << 0;
								out_param << ",";
								out_param << 0;
								out_param << ",";
								out_param << 0;
								out_param << "\n";
							}
							else if (NR == 1){
								out_param << 0;
								out_param << ",";
								out_param << 0;
								out_param << ",";
								out_param << 1;
								out_param << ",";
								out_param << 0;
								out_param << ",";
								out_param << 0;
								out_param << ",";
								out_param << 0;
								out_param << "\n";
							}
							else if (NC == 1){
								out_param << 0;
								out_param << ",";
								out_param << 0;
								out_param << ",";
								out_param << 0;
								out_param << ",";
								out_param << 0;
								out_param << ",";
								out_param << 1;
								out_param << ",";
								out_param << 0;
								out_param << "\n";
							}
						}*/
					}
				}
			}
		}
	}
}

void GLWidget3D::generateFacadeImages(QString facadeImagesPath, int imageNum, bool bDataAugmentaion, int width, int height, std::pair<int, int> imageRows, std::pair<int, int> imageCols, std::pair<int, int> imageGroups, std::pair<double, double> imageRelativeWidth, std::pair<double, double> imageRelativeHeight, bool bWindowDis, double windowDisRatio, bool bWindowProb, double windowProb, bool bPadding, std::pair<int, int> imagePadding, std::pair<int, int> imageDoors, std::pair<double, double> imageDRelativeWidth, std::pair<double, double> imageDRelativeHeight){
	/*
	std::cout << "facadeImagesPath is " << facadeImagesPath.toUtf8().constData() << std::endl;
	std::cout << "imageNum is " << imageNum << std::endl;
	std::cout <<"bDataAugmentaion is " << bDataAugmentaion << std::endl;
	std::cout << "width is " << width << std::endl;
	std::cout << "height is " << height << std::endl;
	std::cout << "imageRows is " << "(" << imageRows.first << ", " << imageRows.second<<")"<< std::endl;
	std::cout << "imageCols is " << "(" << imageCols.first << ", " << imageCols.second << ")" << std::endl;
	std::cout << "imageGroups is " << "(" << imageGroups.first << ", " << imageGroups.second << ")" << std::endl;
	std::cout << "imageRelativeWidth is " << "(" << imageRelativeWidth.first << ", " << imageRelativeWidth.second << ")" << std::endl;
	std::cout << "imageRelativeHeight is " << "(" << imageRelativeHeight.first << ", " << imageRelativeHeight.second << ")" << std::endl;
	std::cout << "bWindowDis is " << bWindowDis << std::endl;
	std::cout << "windowDisRatio is " << windowDisRatio << std::endl;
	std::cout << "bWindowProb is " << bWindowProb << std::endl;
	std::cout << "windowProb is " << windowProb << std::endl;
	std::cout << "bPadding is " << bPadding << std::endl;
	std::cout << "imagePadding is " << "(" << imagePadding.first << ", " << imagePadding.second << ")" << std::endl;
	std::cout << "imageDoors is " << "(" << imageDoors.first << ", " << imageDoors.second << ")" << std::endl;
	std::cout << "imageDRelativeWidth is " << "(" << imageDRelativeWidth.first << ", " << imageDRelativeWidth.second << ")" << std::endl;
	std::cout << "imageDRelativeHeight is " << "(" << imageDRelativeHeight.first << ", " << imageDRelativeHeight.second << ")" << std::endl;*/
	

	// generate facade images
	int index = 0;
	std::ofstream out_param(facadeImagesPath.toUtf8() + "/parameters.txt", std::ios::app);
	for (int l = 0; l < 2000; l++){
		cv::Scalar bg_color(255, 255, 255); // white back ground
		cv::Scalar window_color(0, 0, 0); // black for windows
		int NR = util::genRand(imageRows.first, imageRows.second + 1);
		int NG = util::genRand(imageGroups.first, imageGroups.second + 1);
		int NC = 0;
		if (NG == 1)
			NC = util::genRand(imageCols.first, imageCols.second + 1);
		else{
			NC = util::genRand(imageCols.first + 1, imageCols.second / NG + 2);
		}
		int ND = util::genRand(imageDoors.first, imageDoors.second + 1);
		double ratioWidth = 0;
		double ratioHeight = 0;
		if (NC == 1){
			ratioWidth = util::genRand(0.9, 1.0);
		}
		else if (NC <= 3){
			ratioWidth = util::genRand(imageRelativeWidth.second - 0.4, imageRelativeWidth.second);
		}
		else{
			ratioWidth = util::genRand(imageRelativeWidth.first, imageRelativeWidth.second);
		}
		ratioHeight = util::genRand(imageRelativeHeight.first, imageRelativeHeight.second);
		double ratioDWidth = util::genRand(imageDRelativeWidth.first, imageDRelativeWidth.second);
		double ratioDHeight = util::genRand(imageDRelativeHeight.first, imageDRelativeHeight.second);
		int thickness = -1;
		double DFW = width * 1.0 / ND;
		double DFH = height * ratioDHeight;
		double DW = DFW * ratioDWidth;
		double DH = DFH * (1 - windowDisRatio);
		double FH = (height - DFH) * 1.0 / NR;
		double FW = width * 1.0 / NC;
		double WH = FH * ratioHeight;
		double WW = FW * ratioWidth;
		std::cout << "NR is " << NR << std::endl;
		std::cout << "NC is " << NC << std::endl;
		std::cout << "FH is " << FH << std::endl;
		std::cout << "FW is " << FW << std::endl;
		std::cout << "NG is " << NG << std::endl;
		std::cout << "ND is " << ND << std::endl;
		std::cout << "ratioWidth is " << ratioWidth << std::endl;
		std::cout << "ratioHeight is " << ratioHeight << std::endl;
		std::cout << "WH is " << WH << std::endl;
		std::cout << "WW is " << WW << std::endl;
		std::cout << "ratioDWidth is " << ratioDWidth << std::endl;
		std::cout << "ratioDHeight is " << ratioDHeight << std::endl;
		std::cout << "DH is " << DH << std::endl;
		std::cout << "DW is " << DW << std::endl;
		// draw facade image
		for (int iter_outers = 0; iter_outers < 5; ++iter_outers){
			cv::Mat result(height, width, CV_8UC3, bg_color);
			if (NG == 1){
				// windows
				for (int i = 0; i < NR; ++i) {
					for (int j = 0; j < NC; ++j) {
						float x1 = (FW - WW) * 0.5 + FW * j;
						float y1 = (FH - WH) * 0.5 + FH * i;
						float x2 = x1 + WW;
						float y2 = y1 + WH;
						//cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), window_color, thickness);
						if (bWindowDis) {
							x1 += util::genRand(-WW * windowDisRatio, WW * windowDisRatio);
							y1 += util::genRand(-WH * windowDisRatio, WH * windowDisRatio);
							x2 += util::genRand(-WW * windowDisRatio, WW * windowDisRatio);
							y2 += util::genRand(-WH * windowDisRatio, WH * windowDisRatio);
						}

						if (bWindowProb){
							if (util::genRand() < windowProb) {
								cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), window_color, thickness);
							}
						}
						else{
							cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), window_color, thickness);
						}
					}
				}
				// doors
				for (int i = 0; i < ND; i++){
					float x1 = (DFW - DW) * 0.5 + DFW * i;
					float y1 = height - DH;
					float x2 = x1 + DW;
					float y2 = y1 + DH;
					if (bWindowDis) {
						x1 += util::genRand(-DW * windowDisRatio, DW * windowDisRatio);
						y1 += util::genRand(-DH * windowDisRatio, DH * windowDisRatio);
						x2 += util::genRand(-DW * windowDisRatio, DW * windowDisRatio);
						y2 += util::genRand(-DH * windowDisRatio, DH * windowDisRatio);
					}
					cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), window_color, thickness);
				}
			}
			else{
				// windows
				double gap = 0.05 * WW; // Assume not too many windows in one group
				double GWW = (WW - gap * (NG - 1)) / NG;
				double GFW = GWW + gap;
				for (int i = 0; i < NR; ++i) {
					for (int j = 0; j < NC; ++j) {
						float x1 = (FW - WW) * 0.5 + FW * j;
						float y1 = (FH - WH) * 0.5 + FH * i;
						for (int k = 0; k < NG; k++){
							float g_x1 = x1 + GFW * k;
							float g_y1 = y1;
							float g_x2 = g_x1 + GWW;
							float g_y2 = g_y1 + WH;

							if (bWindowDis) {
								g_x1 += util::genRand(-GWW * windowDisRatio, GWW * windowDisRatio);
								g_y1 += util::genRand(-WH * windowDisRatio, WH * windowDisRatio);
								g_x2 += util::genRand(-GWW * windowDisRatio, GWW * windowDisRatio);
								g_y2 += util::genRand(-WH * windowDisRatio, WH * windowDisRatio);
							}

							if (bWindowProb){
								if (util::genRand() < windowProb) {
									cv::rectangle(result, cv::Point(std::round(g_x1), std::round(g_y1)), cv::Point(std::round(g_x2), std::round(g_y2)), window_color, thickness);
								}
							}
							else{
								cv::rectangle(result, cv::Point(std::round(g_x1), std::round(g_y1)), cv::Point(std::round(g_x2), std::round(g_y2)), window_color, thickness);
							}
						}
					}
				}
				// doors
				for (int i = 0; i < ND; i++){
					float x1 = (DFW - DW) * 0.5 + DFW * i;
					float y1 = height - DH;
					float x2 = x1 + DW;
					float y2 = y1 + DH;
					if (bWindowDis) {
						x1 += util::genRand(-DW * windowDisRatio, DW * windowDisRatio);
						y1 += util::genRand(-DH * windowDisRatio, DH * windowDisRatio);
						x2 += util::genRand(-DW * windowDisRatio, DW * windowDisRatio);
						y2 += util::genRand(-DH * windowDisRatio, DH * windowDisRatio);
					}
					cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), window_color, thickness);
				}
			}

			QString img_filename = facadeImagesPath + QString("/facade_image_%1.png").arg(index, 6, 10, QChar('0'));
			QString img_name = QString("facade_image_%1.png").arg(index, 6, 10, QChar('0'));

			// add padding
			if (bPadding){
				int top = util::genRand(imagePadding.first, imagePadding.second + 1);
				int bottom = util::genRand(imagePadding.first, imagePadding.second + 1);
				int left = util::genRand(imagePadding.first, imagePadding.second + 1);
				int right = util::genRand(imagePadding.first, imagePadding.second + 1);
				/*std::cout << "top is " << top << std::endl;
				std::cout << "bottom is " << bottom << std::endl;
				std::cout << "left is " << left << std::endl;
				std::cout << "right is " << right << std::endl;*/
				int borderType = cv::BORDER_CONSTANT;
				cv::copyMakeBorder(result, result, top, bottom, left, right, borderType, bg_color);
				// crop
				{
					int final_width = width + 2 * imagePadding.first;
					int final_height = height + 2 * imagePadding.first;
					int x1, y1;
					x1 = rand() % (int)(result.cols - final_width + 1);
					y1 = rand() % (int)(result.rows - final_height + 1);
					/*std::cout << "x1 is " << x1 << std::endl;
					std::cout << "y1 is " << y1 << std::endl;*/
					result = cv::Mat(result, cv::Rect(x1, y1, final_width, final_height)).clone();
				}
			}
			// add rotation
			if (false)
			{
				cv::cvtColor(result, result, cv::COLOR_BGR2GRAY);
				// rotate the image
				cv::Mat rot_img;
				cv::Point2f offset(result.cols / 2 + rand() % 30 - 15, result.rows / 2 + rand() % 30 - 15);
				float angle = rand() % 5 - 2;
				cv::Mat rot_mat = cv::getRotationMatrix2D(offset, angle, 1.0);
				cv::warpAffine(result, rot_img, rot_mat, result.size(), cv::INTER_CUBIC, cv::BORDER_REPLICATE);
				cv::cvtColor(rot_img, result, cv::COLOR_GRAY2BGR);
			}
			cv::imwrite(img_filename.toUtf8().constData(), result);
			index++;
			// write to parameters.txt
			//{
			//	// normalize for NN training
			//	out_param << img_name.toUtf8().constData();
			//	out_param << ",";
			//	out_param << (NR - imageRows.first) * 1.0 / (imageRows.second - imageRows.first);
			//	/*out_param << ",";
			//	out_param << (NC - imageCols.first) * 1.0 / (imageCols.second - imageCols.first);*/
			//	out_param << ",";
			//	out_param << (ND - imageDoors.first) * 1.0 / (imageDoors.second - imageDoors.first);
			//	/*out_param << ",";
			//	out_param << (ratioWidth - imageRelativeWidth.first) * 1.0 / (imageRelativeWidth.second - imageRelativeWidth.first);*/
			//	out_param << ",";
			//	out_param << (ratioHeight - imageRelativeHeight.first) * 1.0 / (imageRelativeHeight.second - imageRelativeHeight.first);
			//	out_param << ",";
			//	out_param << (ratioDWidth - imageDRelativeWidth.first) * 1.0 / (imageDRelativeWidth.second - imageDRelativeWidth.first);
			//	out_param << ",";
			//	out_param << (ratioDHeight - imageDRelativeHeight.first) * 1.0 / (imageDRelativeHeight.second - imageDRelativeHeight.first);
			//	out_param << "\n";
			//}
			{
				out_param << img_name.toUtf8().constData();
				out_param << ",";
				if (NR > 1 && NC > 1){
					out_param << 0;
					out_param << ",";
					out_param << 1;
					out_param << ",";
					out_param << 0;
					out_param << ",";
					out_param << 0;
					out_param << ",";
					out_param << 0;
					out_param << ",";
					out_param << 0;
					out_param << "\n";
				}
				else if (NR == 1){
					out_param << 0;
					out_param << ",";
					out_param << 0;
					out_param << ",";
					out_param << 0;
					out_param << ",";
					out_param << 1;
					out_param << ",";
					out_param << 0;
					out_param << ",";
					out_param << 0;
					out_param << "\n";
				}
				else if (NC == 1){
					out_param << 0;
					out_param << ",";
					out_param << 0;
					out_param << ",";
					out_param << 0;
					out_param << ",";
					out_param << 0;
					out_param << ",";
					out_param << 0;
					out_param << ",";
					out_param << 1;
					out_param << "\n";
				}
			}
		}
	}
}

void GLWidget3D::generateFacadeImages(QString facadeImagesPath, bool bDataAugmentaion, int width, int height, std::pair<int, int> imageRows, std::pair<int, int> imageCols, std::pair<int, int> imageGroups, std::pair<double, double> imageRelativeWidth, std::pair<double, double> imageRelativeHeight, bool bWindowDis, double windowDisRatio, bool bWindowProb, double windowProb, bool bPadding, std::pair<int, int> imagePadding, std::pair<int, int> imageDoors, std::pair<double, double> imageDRelativeWidth, std::pair<double, double> imageDRelativeHeight){
	/*
	std::cout << "facadeImagesPath is " << facadeImagesPath.toUtf8().constData() << std::endl;
	std::cout << "imageNum is " << imageNum << std::endl;
	std::cout <<"bDataAugmentaion is " << bDataAugmentaion << std::endl;
	std::cout << "width is " << width << std::endl;
	std::cout << "height is " << height << std::endl;
	std::cout << "imageRows is " << "(" << imageRows.first << ", " << imageRows.second<<")"<< std::endl;
	std::cout << "imageCols is " << "(" << imageCols.first << ", " << imageCols.second << ")" << std::endl;
	std::cout << "imageGroups is " << "(" << imageGroups.first << ", " << imageGroups.second << ")" << std::endl;
	std::cout << "imageRelativeWidth is " << "(" << imageRelativeWidth.first << ", " << imageRelativeWidth.second << ")" << std::endl;
	std::cout << "imageRelativeHeight is " << "(" << imageRelativeHeight.first << ", " << imageRelativeHeight.second << ")" << std::endl;
	std::cout << "bWindowDis is " << bWindowDis << std::endl;
	std::cout << "windowDisRatio is " << windowDisRatio << std::endl;
	std::cout << "bWindowProb is " << bWindowProb << std::endl;
	std::cout << "windowProb is " << windowProb << std::endl;
	std::cout << "bPadding is " << bPadding << std::endl;
	std::cout << "imagePadding is " << "(" << imagePadding.first << ", " << imagePadding.second << ")" << std::endl;
	std::cout << "imageDoors is " << "(" << imageDoors.first << ", " << imageDoors.second << ")" << std::endl;
	std::cout << "imageDRelativeWidth is " << "(" << imageDRelativeWidth.first << ", " << imageDRelativeWidth.second << ")" << std::endl;
	std::cout << "imageDRelativeHeight is " << "(" << imageDRelativeHeight.first << ", " << imageDRelativeHeight.second << ")" << std::endl;*/

	std::cout << "imageDRelativeWidth is " << "(" << imageDRelativeWidth.first << ", " << imageDRelativeWidth.second << ")" << std::endl;
	std::cout << "imageDRelativeHeight is " << "(" << imageDRelativeHeight.first << ", " << imageDRelativeHeight.second << ")" << std::endl;
	// generate facade images
	// generate facade images
	int index = 48;
	double step_W = 0.1;
	double step_H = 0.1;
	double step_DW = 0.1;
	double step_DH = 0.1;
	int num_W = 0;
	int num_H = 0;
	int num_DW = 0;
	int num_DH = 0;
	if (ceil((imageRelativeWidth.second - imageRelativeWidth.first) / step_W) - (imageRelativeWidth.second - imageRelativeWidth.first) / step_W < 0.01)
		num_W = ceil((imageRelativeWidth.second - imageRelativeWidth.first) / step_W);
	else
		num_W = floor((imageRelativeWidth.second - imageRelativeWidth.first) / step_W);

	if (ceil((imageRelativeHeight.second - imageRelativeHeight.first) / step_H) - (imageRelativeHeight.second - imageRelativeHeight.first) / step_H < 0.01)
		num_H = ceil((imageRelativeHeight.second - imageRelativeHeight.first) / step_H);
	else
		num_H = floor((imageRelativeHeight.second - imageRelativeHeight.first) / step_H);

	if (ceil((imageDRelativeWidth.second - imageDRelativeWidth.first) / step_DW) - (imageDRelativeWidth.second - imageDRelativeWidth.first) / step_DW < 0.01)
		num_DW = ceil((imageDRelativeWidth.second - imageDRelativeWidth.first) / step_DW);
	else
		num_DW = floor((imageDRelativeWidth.second - imageDRelativeWidth.first) / step_DW);

	if (ceil((imageDRelativeHeight.second - imageDRelativeHeight.first) / step_DH) - (imageDRelativeHeight.second - imageDRelativeHeight.first) / step_DH < 0.01)
		num_DH = ceil((imageDRelativeHeight.second - imageDRelativeHeight.first) / step_DH);
	else
		num_DH = floor((imageDRelativeHeight.second - imageDRelativeHeight.first) / step_DH);

	std::cout << "num_W is " << num_W << std::endl;
	std::cout << "num_H is " << num_H << std::endl;
	std::cout << "num_DW is " << num_DW << std::endl;
	std::cout << "num_DH is " << num_DH << std::endl;
	cv::Scalar bg_color(0, 0, 0); // white back ground
	cv::Scalar window_color(255, 255, 255); // black for windows
	int thickness = -1;
	std::ofstream out_param(facadeImagesPath.toUtf8() + "/parameters.txt", std::ios::app);
	for (int row = imageRows.first; row <= imageRows.second; row++){ // loop row
		for (int col = imageCols.first; col <= imageCols.second; col++){ // loop col
			for (int door = imageDoors.first; door <= imageDoors.second; door++){
				for (int relativeW = 0; relativeW <= num_W; relativeW++){ // loop relativeWidth
					for (int relativeH = 0; relativeH <= num_H; relativeH++){
						for (int relativeDW = 0; relativeDW <= num_DW; relativeDW++){
							for (int relativeDH = 0; relativeDH <= num_DH; relativeDH++){
								int NR = row;
								int NG = 1;
								int NC = col;
								int ND = door;
								double ratioWidth = relativeW * step_W + imageRelativeWidth.first;
								double ratioHeight = relativeH * step_H + imageRelativeHeight.first;
								double ratioDWidth = relativeDW * step_DW + imageDRelativeWidth.first;
								double ratioDHeight = relativeDH * step_DH + imageDRelativeHeight.first;
								double DFW = width * 1.0 / ND;
								double DFH = height * ratioDHeight;
								double DW = DFW * ratioDWidth;
								double DH = DFH * (1 - windowDisRatio);
								double GAP = height * util::genRand(0, 0.15);
								double FH = (height - DFH - GAP) * 1.0 / NR;
								double FW = width * 1.0 / NC;
								double WH = FH * ratioHeight;
								double WW = FW * ratioWidth;
								if (ND > 1)
									DFW = DW + (width - DW * ND) / (ND - 1);
								if (NC > 1)
									FW = WW + (width - WW * NC) / (NC - 1);
								std::cout << "NR is " << NR << std::endl;
								std::cout << "NC is " << NC << std::endl;
								std::cout << "FH is " << FH << std::endl;
								std::cout << "FW is " << FW << std::endl;
								std::cout << "NG is " << NG << std::endl;
								std::cout << "ND is " << ND << std::endl;
								std::cout << "ratioWidth is " << ratioWidth << std::endl;
								std::cout << "ratioHeight is " << ratioHeight << std::endl;
								std::cout << "WH is " << WH << std::endl;
								std::cout << "WW is " << WW << std::endl;
								std::cout << "ratioDWidth is " << ratioDWidth << std::endl;
								std::cout << "ratioDHeight is " << ratioDHeight << std::endl;
								std::cout << "DH is " << DH << std::endl;
								std::cout << "DW is " << DW << std::endl;

								int num_iters = 4;
								if (row >= 6 && col >= 6)
									num_iters = 8;
								// draw facade image
								for (int iter_outers = 0; iter_outers < 1; ++iter_outers){
									cv::Mat result(height, width, CV_8UC1, bg_color);
									if (NG == 1){
										// windows
										for (int i = 0; i < NR; ++i) {
											for (int j = 0; j < NC; ++j) {
												float x1 = FW * j;
												float y1 = FH * i;
												float x2 = x1 + WW;
												float y2 = y1 + WH;
												//cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), window_color, thickness);
												if (bWindowDis) {
													x1 += util::genRand(-WW * windowDisRatio, WW * windowDisRatio);
													y1 += util::genRand(-WH * windowDisRatio, WH * windowDisRatio);
													x2 += util::genRand(-WW * windowDisRatio, WW * windowDisRatio);
													y2 += util::genRand(-WH * windowDisRatio, WH * windowDisRatio);
												}

												if (bWindowProb){
													if (util::genRand() < windowProb) {
														cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), window_color, thickness);
													}
												}
												else{
													cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), window_color, thickness);
												}
											}
										}
										// doors
										for (int i = 0; i < ND; i++){
											float x1 = DFW * i;
											float y1 = height - DH;
											float x2 = x1 + DW;
											float y2 = y1 + DH;
											if (bWindowDis) {
												x1 += util::genRand(-DW * windowDisRatio, DW * windowDisRatio);
												y1 += util::genRand(-DH * windowDisRatio, DH * windowDisRatio);
												x2 += util::genRand(-DW * windowDisRatio, DW * windowDisRatio);
												y2 += util::genRand(-DH * windowDisRatio, DH * windowDisRatio);
											}
											cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), window_color, thickness);
										}
									}
									QString img_filename = facadeImagesPath + QString("/facade_%1.png").arg(index, 5, 10, QChar('0'));
									QString img_name = QString("facade_%1.png").arg(index, 5, 10, QChar('0'));

									// add padding
									if (bPadding){
										int top = util::genRand(imagePadding.first, imagePadding.second + 1);
										int bottom = util::genRand(imagePadding.first, imagePadding.second + 1);
										int left = util::genRand(imagePadding.first, imagePadding.second + 1);
										int right = util::genRand(imagePadding.first, imagePadding.second + 1);
										int borderType = cv::BORDER_CONSTANT;
										cv::copyMakeBorder(result, result, top, bottom, left, right, borderType, bg_color);
									}
									cv::imwrite(img_filename.toUtf8().constData(), result);
									index++;
									// write to parameters.txt
									//{
									//	// normalize for NN training
									//	out_param << img_name.toUtf8().constData();
									//	out_param << ",";
									//	out_param << (NR - imageRows.first) * 1.0 / (imageRows.second - imageRows.first);
									//	/*out_param << ",";
									//	out_param << (NC - imageCols.first) * 1.0 / (imageCols.second - imageCols.first);*/
									//	out_param << ",";
									//	out_param << (ND - imageDoors.first) * 1.0 / (imageDoors.second - imageDoors.first);
									//	/*out_param << ",";
									//	out_param << (ratioWidth - imageRelativeWidth.first) * 1.0 / (imageRelativeWidth.second - imageRelativeWidth.first);*/
									//	out_param << ",";
									//	out_param << (ratioHeight - imageRelativeHeight.first) * 1.0 / (imageRelativeHeight.second - imageRelativeHeight.first);
									//	out_param << ",";
									//	out_param << (ratioDWidth - imageDRelativeWidth.first) * 1.0 / (imageDRelativeWidth.second - imageDRelativeWidth.first);
									//	out_param << ",";
									//	out_param << (ratioDHeight - imageDRelativeHeight.first) * 1.0 / (imageDRelativeHeight.second - imageDRelativeHeight.first);
									//	out_param << "\n";
									//}
									{
										out_param << img_name.toUtf8().constData();
										out_param << ",";
										if (NR > 1 && NC > 1){
											out_param << 0;
											out_param << ",";
											out_param << 1;
											out_param << ",";
											out_param << 0;
											out_param << ",";
											out_param << 0;
											out_param << ",";
											out_param << 0;
											out_param << ",";
											out_param << 0;
											out_param << "\n";
										}
										else if (NR == 1){
											out_param << 0;
											out_param << ",";
											out_param << 0;
											out_param << ",";
											out_param << 0;
											out_param << ",";
											out_param << 1;
											out_param << ",";
											out_param << 0;
											out_param << ",";
											out_param << 0;
											out_param << "\n";
										}
										else if (NC == 1){
											out_param << 0;
											out_param << ",";
											out_param << 0;
											out_param << ",";
											out_param << 0;
											out_param << ",";
											out_param << 0;
											out_param << ",";
											out_param << 0;
											out_param << ",";
											out_param << 1;
											out_param << "\n";
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

cv::Mat GLWidget3D::generateFacadeSynImage(int width, int height, int imageRows, int imageCols, int imageGroups, double imageRelativeWidth, double imageRelativeHeight){
	cv::Scalar bg_color(255, 255, 255); // white back ground
	cv::Scalar window_color(0, 0, 0); // black for windows
	int NR = imageRows;
	int NC = imageCols;
	int NG = imageGroups;
	double ratioWidth = imageRelativeWidth;
	double ratioHeight = imageRelativeHeight;
	int thickness = -1;
	cv::Mat result(height, width, CV_8UC3, bg_color);
	double FH = height * 1.0 / NR;
	double FW = width * 1.0 / NC;
	double WH = FH * ratioHeight;
	double WW = FW * ratioWidth;
	std::cout << "NR is " << NR << std::endl;
	std::cout << "NC is " << NC << std::endl;
	std::cout << "NG is " << NG << std::endl;
	std::cout << "FH is " << FH << std::endl;
	std::cout << "FW is " << FW << std::endl;
	std::cout << "ratioWidth is " << ratioWidth << std::endl;
	std::cout << "ratioHeight is " << ratioHeight << std::endl;
	std::cout << "WH is " << WH << std::endl;
	std::cout << "WW is " << WW << std::endl;
	// draw facade image
	if (NG == 1){
		for (int i = 0; i < NR; ++i) {
			for (int j = 0; j < NC; ++j) {
				float x1 = (FW - WW) * 0.5 + FW * j;
				float y1 = (FH - WH) * 0.5 + FH * i;
				float x2 = x1 + WW;
				float y2 = y1 + WH;
				cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), window_color, thickness);
			}
		}
	}
	else{
		double gap = 0.1 * WW; // Assume not too many windows in one group
		double GWW = (WW - gap * (NG - 1)) / NG;
		double GFW = GWW + gap;
		for (int i = 0; i < NR; ++i) {
			for (int j = 0; j < NC; ++j) {
				float x1 = (FW - WW) * 0.5 + FW * j;
				float y1 = (FH - WH) * 0.5 + FH * i;
				for (int k = 0; k < NG; k++){
					float g_x1 = x1 + GFW * k;
					float g_y1 = y1;
					float g_x2 = g_x1 + GWW;
					float g_y2 = g_y1 + WH;

					cv::rectangle(result, cv::Point(std::round(g_x1), std::round(g_y1)), cv::Point(std::round(g_x2), std::round(g_y2)), window_color, thickness);
				}
			}
		}
	}
	return result;
}

void GLWidget3D::generateRoofImages(QString roofImagesPath, int imageNum, bool bDataAugmentaion, int roofType, int width, int height, std::pair<int, int> roofWidth, std::pair<double, double> roofAspect, std::pair<double, double> roofSlope, std::pair<double, double> roofRidge, bool bRoofDis, double roofDisRatio){
	/*std::cout << "roofImagesPath is " << roofImagesPath.toUtf8().constData() << std::endl;
	std::cout << "imageNum is " << imageNum << std::endl;
	std::cout <<"bDataAugmentaion is " << bDataAugmentaion << std::endl;
	std::cout << "width is " << width << std::endl;
	std::cout << "height is " << height << std::endl;
	std::cout << "roofType is " << roofType << std::endl;
	std::cout << "roofWidth is " << "(" << roofWidth.first << ", " << roofWidth.second << ")" << std::endl;
	std::cout << "roofAspect is " << "(" << roofAspect.first << ", " << roofAspect.second << ")" << std::endl;
	std::cout << "roofSlope is " << "(" << roofSlope.first << ", " << roofSlope.second << ")" << std::endl;
	std::cout << "roofRidge is " << "(" << roofRidge.first << ", " << roofRidge.second << ")" << std::endl;
	std::cout << "bWindowDis is " << bRoofDis << std::endl;
	std::cout << "windowDisRatio is " << roofDisRatio << std::endl;*/

	std::ofstream out_param(roofImagesPath.toUtf8() + "/parameters.txt", std::ios::app);
	int index = 0;
	for (int l = 0; l < imageNum; l++){
		cv::Scalar bg_color(0, 0, 0); // white back ground
		int imageRoofWidth = util::genRand(roofWidth.first, roofWidth.second + 1);
		double imageRoofAspect = util::genRand(roofAspect.first, roofAspect.second);
		int imageRoofHeight = imageRoofWidth * imageRoofAspect;
		std::cout << "imageRoofAspect is " << imageRoofAspect << std::endl;
		std::cout << "imageRoofWidth is " << imageRoofWidth << std::endl;
		std::cout << "imageRoofHeight is " << imageRoofHeight << std::endl;
		if (roofType == 0){ // flat roof
			double roofBaseHeight = 2.0; // 2.0 meters
			for (int iter_outers = 0; iter_outers < 5; ++iter_outers){
				cv::Mat result(height, width, CV_8UC1, bg_color);
				int upper_left_w = (width - imageRoofWidth) * 0.5;
				int upper_left_h = (height - imageRoofHeight) * 0.5;
				for (int i = upper_left_h; i < upper_left_h + imageRoofHeight; i++) {
					for (int j = upper_left_w; j < upper_left_w + imageRoofWidth; j++) {
						double pixel_depth = roofBaseHeight;
						if (bRoofDis) {
							pixel_depth += util::genRand(-roofBaseHeight * roofDisRatio, roofBaseHeight * roofDisRatio);
						}
						result.at<uchar>(i, j) = (uchar) 255 * pixel_depth / (1.5 * roofBaseHeight);
					}
				}
				QString img_filename = roofImagesPath + QString("/facade_image_%1.png").arg(index, 6, 10, QChar('0'));
				QString img_name = QString("facade_image_%1.png").arg(index, 6, 10, QChar('0'));
				cv::imwrite(img_filename.toUtf8().constData(), result);
				index++;
				// output to .txt
				{
					out_param << img_name.toUtf8().constData();
					out_param << ",";
					out_param << roofType;
					out_param << ",";
					out_param << imageRoofWidth;
					out_param << ",";
					out_param << imageRoofAspect;
					out_param << "\n";
				}
			}
		}
		else if (roofType == 1){ // gable roof
			double imageRoofSlope = util::genRand(roofSlope.first, roofSlope.second);
			std::cout << "imageRoofSlope is " << imageRoofSlope << std::endl;
			double ridge_height = (1 + roofDisRatio) * imageRoofHeight * 0.5 * tan(imageRoofSlope * PI / 180.0);
			for (int iter_outers = 0; iter_outers < 5; ++iter_outers){
				cv::Mat result(height, width, CV_8UC1, bg_color);
				int upper_left_w = (width - imageRoofWidth) * 0.5;
				int upper_left_h = (height - imageRoofHeight) * 0.5;
				for (int i = 0; i < imageRoofHeight; i++) {
					for (int j = 0; j < imageRoofWidth; j++) {
						double pixel_depth = 0;
						if (i <= imageRoofHeight * 0.5)
							pixel_depth = i *  tan(imageRoofSlope * PI / 180.0);
						else
							pixel_depth = (imageRoofHeight - i) *  tan(imageRoofSlope * PI / 180.0);
						if (bRoofDis) {
							pixel_depth += util::genRand(-pixel_depth * roofDisRatio, pixel_depth * roofDisRatio);
						}
						result.at<uchar>(i + upper_left_h, j + upper_left_w) = (uchar)255 * pixel_depth / ridge_height;
					}
				}
				QString img_filename = roofImagesPath + QString("/facade_image_%1.png").arg(index, 6, 10, QChar('0'));
				QString img_name = QString("facade_image_%1.png").arg(index, 6, 10, QChar('0'));
				cv::imwrite(img_filename.toUtf8().constData(), result);
				index++;
				// output to .txt
				{
					out_param << img_name.toUtf8().constData();
					out_param << ",";
					out_param << roofType;
					out_param << ",";
					out_param << imageRoofWidth;
					out_param << ",";
					out_param << imageRoofAspect;
					out_param << ",";
					out_param << imageRoofSlope;
					out_param << "\n";
				}
			}
		}
		else if (roofType == 2){ // hip roof
			double imageRoofSlope = util::genRand(roofSlope.first, roofSlope.second);
			std::cout << "imageRoofSlope is " << imageRoofSlope << std::endl;
			int imageRoofRidge = imageRoofWidth * util::genRand(roofRidge.first, roofRidge.second);
			std::cout << "imageRoofRidge is " << imageRoofRidge << std::endl;
			double ridge_height = (1 + roofDisRatio) * imageRoofHeight * 0.5 * tan(imageRoofSlope * PI / 180.0);
			double ridge_height_abs = imageRoofHeight * 0.5 * tan(imageRoofSlope * PI / 180.0);
			for (int iter_outers = 0; iter_outers < 5; ++iter_outers){
				cv::Mat result(height, width, CV_8UC1, bg_color);
				// draw outer part
				int upper_left_w = (width - imageRoofWidth) * 0.5;
				int upper_left_h = (height - imageRoofHeight) * 0.5;
				//
				cv::Point2f a1(upper_left_h, upper_left_w);
				cv::Point2f b1(upper_left_h + imageRoofHeight, upper_left_w);
				cv::Point2f c1(upper_left_h + imageRoofHeight * 0.5, (width - imageRoofRidge) * 0.5);
				cv::Point2f a2(upper_left_h, upper_left_w + imageRoofWidth);
				cv::Point2f b2(upper_left_h + imageRoofHeight, upper_left_w + imageRoofWidth);
				cv::Point2f c2(upper_left_h + imageRoofHeight * 0.5, (width - imageRoofRidge) * 0.5 + imageRoofRidge);
				for (int i = 0; i < imageRoofHeight; i++) {
					for (int j = 0; j < imageRoofWidth; j++) {
						cv::Point2f p(i + upper_left_h, j + upper_left_w);
						double pixel_depth = 0;
						if (util::isInside(a1, b1, c1, p)){
							pixel_depth = ridge_height_abs * j / (c1.y - upper_left_w);
						}
						else if (util::isInside(a2, b2, c2, p)){
							pixel_depth = ridge_height_abs * (imageRoofWidth - j) / (upper_left_w + imageRoofWidth - c2.y);
						}
						else{
							if (i <= imageRoofHeight * 0.5)
								pixel_depth = i *  tan(imageRoofSlope * PI / 180.0);
							else
								pixel_depth = (imageRoofHeight - i) *  tan(imageRoofSlope * PI / 180.0);
						}
						if (bRoofDis) {
							pixel_depth += util::genRand(-pixel_depth * roofDisRatio, pixel_depth * roofDisRatio);
						}
						result.at<uchar>(i + upper_left_h, j + upper_left_w) = (uchar)255 * pixel_depth / ridge_height;
					}
				}

				QString img_filename = roofImagesPath + QString("/facade_image_%1.png").arg(index, 6, 10, QChar('0'));
				QString img_name = QString("facade_image_%1.png").arg(index, 6, 10, QChar('0'));
				cv::imwrite(img_filename.toUtf8().constData(), result);
				index++;
				// output to .txt
				{
					out_param << img_name.toUtf8().constData();
					out_param << ",";
					out_param << roofType;
					out_param << ",";
					out_param << imageRoofWidth;
					out_param << ",";
					out_param << imageRoofAspect;
					out_param << ",";
					out_param << imageRoofSlope;
					out_param << ",";
					out_param << imageRoofRidge;
					out_param << "\n";
				}
			}
		}
		else if (roofType == 3){ // pyramidal roof
			double imageRoofSlope = util::genRand(roofSlope.first, roofSlope.second);
			std::cout << "imageRoofSlope is " << imageRoofSlope << std::endl;
			double rof_top_height = (1 + roofDisRatio) * imageRoofHeight * 0.5 * tan(imageRoofSlope * PI / 180.0);
			double rof_top_height_abs = imageRoofHeight * 0.5 * tan(imageRoofSlope * PI / 180.0);
			for (int iter_outers = 0; iter_outers < 5; ++iter_outers){
				cv::Mat result(height, width, CV_8UC1, bg_color);
				// draw outer part
				int upper_left_w = (width - imageRoofWidth) * 0.5;
				int upper_left_h = (height - imageRoofHeight) * 0.5;
				//
				cv::Point2f a1(upper_left_h, upper_left_w);
				cv::Point2f b1(upper_left_h + imageRoofHeight, upper_left_w);
				cv::Point2f c1(upper_left_h + imageRoofHeight * 0.5, width * 0.5);
				cv::Point2f a2(upper_left_h, upper_left_w + imageRoofWidth);
				cv::Point2f b2(upper_left_h + imageRoofHeight, upper_left_w + imageRoofWidth);
				cv::Point2f c2(upper_left_h + imageRoofHeight * 0.5, width * 0.5);
				for (int i = 0; i < imageRoofHeight; i++) {
					for (int j = 0; j < imageRoofWidth; j++) {
						cv::Point2f p(i + upper_left_h, j + upper_left_w);
						double pixel_depth = 0;
						if (util::isInside(a1, b1, c1, p)){
							pixel_depth = rof_top_height_abs * j / (c1.y - upper_left_w);
						}
						else if (util::isInside(a2, b2, c2, p)){
							pixel_depth = rof_top_height_abs * (imageRoofWidth - j) / (upper_left_w + imageRoofWidth - c2.y);
						}
						else{
							if (i <= imageRoofHeight * 0.5)
								pixel_depth = i *  tan(imageRoofSlope * PI / 180.0);
							else
								pixel_depth = (imageRoofHeight - i) *  tan(imageRoofSlope * PI / 180.0);
						}
						if (bRoofDis) {
							pixel_depth += util::genRand(-pixel_depth * roofDisRatio, pixel_depth * roofDisRatio);
						}
						result.at<uchar>(i + upper_left_h, j + upper_left_w) = (uchar)255 * pixel_depth / rof_top_height;
					}
				}

				QString img_filename = roofImagesPath + QString("/facade_image_%1.png").arg(index, 6, 10, QChar('0'));
				QString img_name = QString("facade_image_%1.png").arg(index, 6, 10, QChar('0'));
				cv::imwrite(img_filename.toUtf8().constData(), result);
				index++;
				// output to .txt
				{
					out_param << img_name.toUtf8().constData();
					out_param << ",";
					out_param << roofType;
					out_param << ",";
					out_param << imageRoofWidth;
					out_param << ",";
					out_param << imageRoofAspect;
					out_param << ",";
					out_param << imageRoofSlope;
					out_param << "\n";
				}
			}
		}
		else{

		}
	}

}

cv::Mat GLWidget3D::generateFacade(int width, int height, int imageRows, int imageCols, int imageGroups, std::pair<double, double> imageRelativeW, std::pair<double, double> imageRelativeSideW, std::pair<double, double> imageRelativeMidW, float window_displacement, float window_prob, int imagePadding){
	cv::Scalar bg_color(0, 0, 0); // white back ground
	cv::Scalar window_color(255, 255, 255); // black for windows
	int thickness = -1;
	cv::Mat result(height, width, CV_8UC1, bg_color);
	if (imagePadding > 0){
		int top = imagePadding;
		int bottom = imagePadding;
		int left = imagePadding;
		int right = imagePadding;
		int borderType = cv::BORDER_CONSTANT;
		cv::copyMakeBorder(result, result, top, bottom, left, right, borderType, bg_color);
	}
	/* draw the facade */
	int NR = imageRows;
	int NG = imageGroups;
	int NC = imageCols;
	double FW = width * 1.0 / NC;
	double FH = height * 1.0 / NR;
	double WW = FW * imageRelativeW.first;
	double WH = FH * imageRelativeW.second;
	double WWS = FW * imageRelativeSideW.first;
	double WHS = FH * imageRelativeSideW.second;
	double WWM = FW * imageRelativeMidW.first;
	double WHM = FH * imageRelativeMidW.second;
	double width_spacing = 0.0f;
	double height_spacing = 0.0f;
	//std::cout << "WW, WH is " << WW << ", " << WH << std::endl;
	//std::cout << "WWS, WHS is " << WWS << ", " << WHS << std::endl;
	//std::cout << "WWM, WHM is " << WWM << ", " << WHM << std::endl;
	if (NC > 1)
		width_spacing = (width - WWS * 2 - WW * (NC - 3) - WWM) / (NC - 1);
	if (NR > 1)
		height_spacing = (height - WH * NR) / (NR - 1);
	if (NG == 1){
		float curH_spacing = 0;
		float curW_spacing = 0;
		for (int i = 0; i < NR; ++i) {
			curW_spacing = 0;
			for (int j = 0; j < NC; ++j) {
				float x1, y1, x2, y2;
				float curW, curH;
				if (j == 0 || j == NC - 1){
					curW = WWS;
					curH = WHS;
				}
				else if (NC % 2 == 1 && j == (NC - 1) / 2){
					curW = WWM;
					curH = WHM;
				}
				else{
					curW = WW;
					curH = WH;
				}
				x1 = curW_spacing;
				y1 = curH_spacing;
				x2 = x1 + curW;
				y2 = y1 + curH;
				curW_spacing += curW + width_spacing;
				if (window_displacement > 0) {
					x1 += util::genRand(-curW * window_displacement, curW * window_displacement);
					y1 += util::genRand(-curH * window_displacement, curH * window_displacement);
					x2 += util::genRand(-curW * window_displacement, curW * window_displacement);
					y2 += util::genRand(-curH * window_displacement, curH * window_displacement);
				}

				if (util::genRand() < window_prob) {
					if (imagePadding > 0){
						cv::rectangle(result, cv::Point(std::round(x1) + imagePadding, std::round(y1) + imagePadding), cv::Point(std::round(x2) + imagePadding, std::round(y2) + imagePadding), window_color, thickness);
					}
					else{
						cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), window_color, thickness);
					}
				}
			}
			curH_spacing += height_spacing + WH;
		}
	}
	return result;
}

std::vector<cv::Mat> GLWidget3D::generateDeformFacade(int width, int height, int imageRows, int imageCols, int imageGroups, std::pair<double, double> imageRelativeW, std::pair<double, double> imageRelativeSideW, std::pair<double, double> imageRelativeMidW, float window_displacement, float window_prob, int imagePadding){
	cv::Scalar bg_color(0, 0, 0); // white back ground
	cv::Scalar window_color(255, 255, 255); // black for windows
	int thickness = -1;
	cv::Mat result(height, width, CV_8UC1, bg_color);
	cv::Mat result_G(height, width, CV_8UC1, bg_color);
	if (imagePadding > 0){
		int top = imagePadding;
		int bottom = imagePadding;
		int left = imagePadding;
		int right = imagePadding;
		int borderType = cv::BORDER_CONSTANT;
		cv::copyMakeBorder(result, result, top, bottom, left, right, borderType, bg_color);
		cv::copyMakeBorder(result_G, result_G, top, bottom, left, right, borderType, bg_color);
	}
	/* draw the facade */
	int NR = imageRows;
	int NG = imageGroups;
	int NC = imageCols;
	double FW = width * 1.0 / NC;
	double FH = height * 1.0 / NR;
	double WW = FW * imageRelativeW.first;
	double WH = FH * imageRelativeW.second;
	double WWS = FW * imageRelativeSideW.first;
	double WHS = FH * imageRelativeSideW.second;
	double WWM = FW * imageRelativeMidW.first;
	double WHM = FH * imageRelativeMidW.second;
	double width_spacing = 0.0f;
	double height_spacing = 0.0f;
	//std::cout << "WW, WH is " << WW << ", " << WH << std::endl;
	/*std::cout << "WWS, WHS is " << WWS << ", " << WHS << std::endl;
	std::cout << "WWM, WHM is " << WWM << ", " << WHM << std::endl;*/
	if (NC > 1)
		width_spacing = (width - WWS * 2 - WW * (NC - 3) - WWM) / (NC - 1);
	if (NR > 1)
		height_spacing = (height - WH * NR) / (NR - 1);
	//std::cout << "width_spacing, height_spacing is " << width_spacing << ", " << height_spacing << std::endl;

	// check valid
	double threshold = 0.05;
	bool bValid_Window = true;
	bool bValid_Spacing = true;
	if (WW <= threshold * width || WH <= threshold * height){
		std::vector<cv::Mat> outputs;
		return outputs;
	}
	if (WWS <= threshold * width || WHS <= threshold * height){
		std::vector<cv::Mat> outputs;
		return outputs;
	}
	if (WWM <= threshold * width || WHM <= threshold * height){
		std::vector<cv::Mat> outputs;
		return outputs;
	}
	if (width_spacing <= threshold * width || height_spacing <= threshold * height){
		std::vector<cv::Mat> outputs;
		return outputs;
	}
	if (NG == 1){
		float curH_spacing = 0;
		float curW_spacing = 0;
		for (int i = 0; i < NR; ++i) {
			curW_spacing = 0;
			for (int j = 0; j < NC; ++j) {
				float x1, y1, x2, y2;
				float x1_G, y1_G, x2_G, y2_G;
				float curW, curH;
				if (j == 0 || j == NC - 1){
					curW = WWS;
					curH = WHS;
				}
				else if (NC % 2 == 1 && j == (NC - 1) / 2){
					curW = WWM;
					curH = WHM;
				}
				else{
					curW = WW;
					curH = WH;
				}
				x1 = curW_spacing;
				y1 = curH_spacing;
				x2 = x1 + curW;
				y2 = y1 + curH;
				// G
				x1_G = curW_spacing;
				y1_G = curH_spacing;
				x2_G = x1_G + curW;
				y2_G = y1_G + curH;
				curW_spacing += curW + width_spacing;
				if (window_displacement > 0) {
					x1 += util::genRand(-curW * window_displacement, curW * window_displacement);
					y1 += util::genRand(-curH * window_displacement, curH * window_displacement);
					x2 += util::genRand(-curW * window_displacement, curW * window_displacement);
					y2 += util::genRand(-curH * window_displacement, curH * window_displacement);

					/*x1 += util::genRand(0, curW * window_displacement);
					y1 += util::genRand(0, curH * window_displacement);
					x2 += util::genRand(-curW * window_displacement, 0);
					y2 += util::genRand(-curH * window_displacement, 0);*/

				}

				if (util::genRand() < window_prob) {
					if (imagePadding > 0){
						cv::rectangle(result, cv::Point(std::round(x1) + imagePadding, std::round(y1) + imagePadding), cv::Point(std::round(x2) + imagePadding, std::round(y2) + imagePadding), window_color, thickness);
						cv::rectangle(result_G, cv::Point(std::round(x1_G) + imagePadding, std::round(y1_G) + imagePadding), cv::Point(std::round(x2_G) + imagePadding, std::round(y2_G) + imagePadding), window_color, thickness);

					}
					else{
						cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), window_color, thickness);
						cv::rectangle(result_G, cv::Point(std::round(x1_G), std::round(y1_G)), cv::Point(std::round(x2_G), std::round(y2_G)), window_color, thickness);

					}
				}
			}
			curH_spacing += height_spacing + WH;
		}
	}
	/*int new_padding = 4;
	if (new_padding > 0){
		int top = new_padding;
		int bottom = new_padding;
		int left = new_padding;
		int right = new_padding;
		int borderType = cv::BORDER_CONSTANT;
		cv::copyMakeBorder(result, result, top, bottom, left, right, borderType, bg_color);
		cv::copyMakeBorder(result_G, result_G, top, bottom, left, right, borderType, bg_color);
	}*/
	std::vector<cv::Mat> outputs;
	outputs.push_back(result);
	outputs.push_back(result_G);
	return outputs;
}

void GLWidget3D::generateEDImages(QString facadeImagesPath, int width, int height, float window_displacement, float window_prob, int padding){
	// generate facade images
	int index = 0;
	double step_W = 0.2;
	double step_H = 0.2;
	int num_W = 0;
	int num_H = 0;
	std::pair<int, int> imageRowsRange(3, 8);
	std::pair<int, int> imageColsRange(3, 8);
	std::pair<int, int> imageGroupsRange(1, 1);
	std::pair<double, double> imageRelativeWidthRange(0.3, 0.7);
	std::pair<double, double> imageRelativeHeightRange(0.3, 0.7);


	if (ceil((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W) - (imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W < 0.01)
		num_W = ceil((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W);
	else
		num_W = floor((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W);

	if (ceil((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H) - (imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H < 0.01)
		num_H = ceil((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H);
	else
		num_H = floor((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H);
	std::cout << "num_W is " << num_W << std::endl;
	std::cout << "num_H is " << num_H << std::endl;
	cv::Scalar bg_color(0, 0, 0); // white back ground
	cv::Scalar window_color(255, 255, 255); // black for windows
	int thickness = -1;
	bool bSideW = false;
	bool bMidW = false;
	double ratioWidth = 0.0;
	double ratioHeight = 0.0;
	double ratioSideWidth = 0.0;
	double ratioSideHeight = 0.0;
	double ratioMidWidth = 0.0;
	double ratioMidHeight = 0.0;
	for (int row = imageRowsRange.first; row <= imageRowsRange.second; row++){ // loop row
		for (int col = imageColsRange.first; col <= imageColsRange.second; col++){ // loop col
			for (int relativeW = 0; relativeW <= num_W; relativeW++){ // loop relativeWidth
				for (int relativeH = 0; relativeH <= num_H; relativeH++){
					for (int relativeSideW = 0; relativeSideW <= num_W; relativeSideW++){ // loop relativeWidth
						for (int relativeMidW = 0; relativeMidW <= num_W; relativeMidW++){ // loop relativeWidth
							if (bSideW && bMidW){
								if (relativeW == relativeSideW || relativeW == relativeMidW)
									continue;
								if (col % 2 == 0)
									continue;
							}
							else if (bSideW && !bMidW){
								if (relativeW == relativeSideW || relativeW != relativeMidW)
									continue;
							}
							else if (!bSideW && bMidW){
								if (relativeW != relativeSideW || relativeW == relativeMidW)
									continue;
								if (col % 2 == 0)
									continue;
							}
							else{
								if (relativeW != relativeSideW || relativeW != relativeMidW)
									continue;
							}
							ratioWidth = relativeW * step_W + imageRelativeWidthRange.first;
							ratioHeight = relativeH * step_H + imageRelativeHeightRange.first;
							ratioSideWidth = relativeSideW * step_W + imageRelativeWidthRange.first;
							ratioSideHeight = relativeH * step_H + imageRelativeHeightRange.first;
							ratioMidWidth = relativeMidW * step_W + imageRelativeWidthRange.first;
							ratioMidHeight = relativeH * step_H + imageRelativeHeightRange.first;

							std::pair<double, double> imageRelativeW(ratioWidth, ratioHeight);
							std::pair<double, double> imageRelativeSideW(ratioSideWidth, ratioSideHeight);
							std::pair<double, double> imageRelativeMidW(ratioMidWidth, ratioMidHeight);
							int num_iters = 10;
							if (row >= 6 && col >= 6)
								num_iters = 30;
							// draw facade image
							for (int iter_outers = 0; iter_outers < 1; ++iter_outers){
								cv::Mat result = generateFacade(width, height, row, col, 1, imageRelativeW, imageRelativeSideW, imageRelativeMidW, window_displacement, window_prob, padding);
								QString img_filename = facadeImagesPath + QString("/facade_image_%1.png").arg(index, 6, 10, QChar('0'));
								std::cout << "img_filename is " << img_filename.toUtf8().constData() << std::endl;
								cv::imwrite(img_filename.toUtf8().constData(), result);
								//cv::imwrite(img_filename_G.toUtf8().constData(), result_G);
								index++;
							}
						}
					}
				}
			}
		}
	}
}

int GLWidget3D::generateDeformImages(QString facadeImagesPath, int index, int width, int height, float window_displacement, float window_prob, int padding){
	// generate facade images
	double step_W = 0.2;
	double step_H = 0.2;
	int num_W = 0;
	int num_H = 0;
	//std::pair<int, int> imageRowsRange(2, 7);
	//std::pair<int, int> imageColsRange(2, 7);
	std::pair<int, int> imageRowsRange(3, 8);
	std::pair<int, int> imageColsRange(3, 8);
	std::pair<int, int> imageGroupsRange(1, 1);
	std::pair<double, double> imageRelativeWidthRange(0.3, 0.7);
	std::pair<double, double> imageRelativeHeightRange(0.3, 0.7);


	if (ceil((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W) - (imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W < 0.01)
		num_W = ceil((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W);
	else
		num_W = floor((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W);

	if (ceil((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H) - (imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H < 0.01)
		num_H = ceil((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H);
	else
		num_H = floor((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H);
	std::cout << "num_W is " << num_W << std::endl;
	std::cout << "num_H is " << num_H << std::endl;
	cv::Scalar bg_color(0, 0, 0); // white back ground
	cv::Scalar window_color(255, 255, 255); // black for windows
	int thickness = -1;
	bool bSideW = false;
	bool bMidW = false;
	double ratioWidth = 0.0;
	double ratioHeight = 0.0;
	double ratioSideWidth = 0.0;
	double ratioSideHeight = 0.0;
	double ratioMidWidth = 0.0;
	double ratioMidHeight = 0.0;
	for (int row = imageRowsRange.first; row <= imageRowsRange.second; row++){ // loop row
		for (int col = imageColsRange.first; col <= imageColsRange.second; col++){ // loop col
			for (int relativeW = 0; relativeW <= num_W; relativeW++){ // loop relativeWidth
				for (int relativeH = 0; relativeH <= num_H; relativeH++){
					for (int relativeSideW = 0; relativeSideW <= num_W; relativeSideW++){ // loop relativeWidth
						for (int relativeMidW = 0; relativeMidW <= num_W; relativeMidW++){ // loop relativeWidth
							if (bSideW && bMidW){
								if (relativeW == relativeSideW || relativeW == relativeMidW)
									continue;
								if (col % 2 == 0)
									continue;
							}
							else if (bSideW && !bMidW){
								if (relativeW == relativeSideW || relativeW != relativeMidW)
									continue;
							}
							else if (!bSideW && bMidW){
								if (relativeW != relativeSideW || relativeW == relativeMidW)
									continue;
								if (col % 2 == 0)
									continue;
							}
							else{
								if (relativeW != relativeSideW || relativeW != relativeMidW)
									continue;
							}
							ratioWidth = relativeW * step_W + imageRelativeWidthRange.first;
							ratioHeight = relativeH * step_H + imageRelativeHeightRange.first;
							ratioSideWidth = relativeSideW * step_W + imageRelativeWidthRange.first;
							ratioSideHeight = relativeH * step_H + imageRelativeHeightRange.first;
							ratioMidWidth = relativeMidW * step_W + imageRelativeWidthRange.first;
							ratioMidHeight = relativeH * step_H + imageRelativeHeightRange.first;

							std::pair<double, double> imageRelativeW(ratioWidth, ratioHeight);
							std::pair<double, double> imageRelativeSideW(ratioSideWidth, ratioSideHeight);
							std::pair<double, double> imageRelativeMidW(ratioMidWidth, ratioMidHeight);
							int num_iters = 10;
							if (row >= 6 && col >= 6)
								num_iters = 30;
							if (int(window_prob) == 1)
								num_iters = 1;
							// draw facade image
							int num_deform = 3;
							if (int(1 - window_displacement) == 1)
								num_deform = 1;

							for (int iter_outers = 0; iter_outers < 3/*num_deform * num_iters*/; ++iter_outers){
								std::vector<cv::Mat> outpus = generateDeformFacade(width, height, row, col, 1, imageRelativeW, imageRelativeSideW, imageRelativeMidW, window_displacement, window_prob, padding);
								if (outpus.size() == 0){
									//std::cout << "invlaid image" << std::endl;
									continue;
								}
								// A
								QString img_filename_A = facadeImagesPath + QString("/A/facade_%1.png").arg(index, 5, 10, QChar('0'));
								//std::cout << "img_filename_A is " << img_filename_A.toUtf8().constData() << std::endl;
								cv::imwrite(img_filename_A.toUtf8().constData(), outpus[0]);
								// G
								QString img_filename_G = facadeImagesPath + QString("/G/facade_%1.png").arg(index, 5, 10, QChar('0'));
								//std::cout << "img_filename_G is " << img_filename_G.toUtf8().constData() << std::endl;
								cv::imwrite(img_filename_G.toUtf8().constData(), outpus[1]);
								index++;
							}
						}
					}
				}
			}
		}
	}
	return index;
}

int GLWidget3D::generateFuseImages(QString facadeImagesPath, int index, int width, int height, float window_displacement, float window_prob, int padding){
	// generate facade images
	double step_W = 0.2;
	double step_H = 0.2;
	int num_W = 0;
	int num_H = 0;
	std::pair<int, int> imageRowsRange(3, 8);
	std::pair<int, int> imageColsRange(3, 8);
	std::pair<int, int> imageGroupsRange(1, 1);
	std::pair<double, double> imageRelativeWidthRange(0.3, 0.7);
	std::pair<double, double> imageRelativeHeightRange(0.3, 0.7);


	if (ceil((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W) - (imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W < 0.01)
		num_W = ceil((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W);
	else
		num_W = floor((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W);

	if (ceil((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H) - (imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H < 0.01)
		num_H = ceil((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H);
	else
		num_H = floor((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H);
	std::cout << "num_W is " << num_W << std::endl;
	std::cout << "num_H is " << num_H << std::endl;
	cv::Scalar bg_color(0, 0, 0); // white back ground
	cv::Scalar window_color(255, 255, 255); // black for windows
	int thickness = -1;
	bool bSideW = false;
	bool bMidW = false;
	double ratioWidth = 0.0;
	double ratioHeight = 0.0;
	double ratioSideWidth = 0.0;
	double ratioSideHeight = 0.0;
	double ratioMidWidth = 0.0;
	double ratioMidHeight = 0.0;
	for (int row = imageRowsRange.first; row <= imageRowsRange.second; row++){ // loop row
		for (int col = imageColsRange.first; col <= imageColsRange.second; col++){ // loop col
			for (int relativeW = 0; relativeW <= num_W; relativeW++){ // loop relativeWidth
				for (int relativeH = 0; relativeH <= num_H; relativeH++){
					for (int relativeSideW = 0; relativeSideW <= num_W; relativeSideW++){ // loop relativeWidth
						for (int relativeMidW = 0; relativeMidW <= num_W; relativeMidW++){ // loop relativeWidth
							if (bSideW && bMidW){
								if (relativeW == relativeSideW || relativeW == relativeMidW)
									continue;
								if (col % 2 == 0)
									continue;
							}
							else if (bSideW && !bMidW){
								if (relativeW == relativeSideW || relativeW != relativeMidW)
									continue;
							}
							else if (!bSideW && bMidW){
								if (relativeW != relativeSideW || relativeW == relativeMidW)
									continue;
								if (col % 2 == 0)
									continue;
							}
							else{
								if (relativeW != relativeSideW || relativeW != relativeMidW)
									continue;
							}
							ratioWidth = relativeW * step_W + imageRelativeWidthRange.first;
							ratioHeight = relativeH * step_H + imageRelativeHeightRange.first;
							ratioSideWidth = relativeSideW * step_W + imageRelativeWidthRange.first;
							ratioSideHeight = relativeH * step_H + imageRelativeHeightRange.first;
							ratioMidWidth = relativeMidW * step_W + imageRelativeWidthRange.first;
							ratioMidHeight = relativeH * step_H + imageRelativeHeightRange.first;

							std::pair<double, double> imageRelativeW(ratioWidth, ratioHeight);
							std::pair<double, double> imageRelativeSideW(ratioSideWidth, ratioSideHeight);
							std::pair<double, double> imageRelativeMidW(ratioMidWidth, ratioMidHeight);
							int num_iters = 10;
							if (row >= 6 && col >= 6)
								num_iters = 30;
							// draw facade image
							for (int iter_outers = 0; iter_outers < 1; ++iter_outers){
								// A
								QString img_filename_A = facadeImagesPath + QString("/A/facade_%1.png").arg(index, 5, 10, QChar('0'));
								std::cout << "img_filename_A is " << img_filename_A.toUtf8().constData() << std::endl;
								cv::Mat result_A = generateFacade(width, height, row, col, 1, imageRelativeW, imageRelativeSideW, imageRelativeMidW, window_displacement, window_prob, padding);
								cv::imwrite(img_filename_A.toUtf8().constData(), result_A);
								// B
								QString img_filename_B = facadeImagesPath + QString("/B/facade_%1.png").arg(index, 5, 10, QChar('0'));
								std::cout << "img_filename_B is " << img_filename_B.toUtf8().constData() << std::endl;
								cv::Mat result_B = generateFacade(width, height, row, col, 1, imageRelativeW, imageRelativeSideW, imageRelativeMidW, window_displacement, window_prob, padding);
								cv::imwrite(img_filename_B.toUtf8().constData(), result_B);
								index++;
							}
						}
					}
				}
			}
		}
	}
	return index;
}

int GLWidget3D::generateFuseImagesTest(QString facadeImagesPath, int index, int width, int height, float window_displacement, float window_prob, int padding){
	// generate facade images
	double step_W = 0.2;
	double step_H = 0.2;
	int num_W = 0;
	int num_H = 0;
	std::pair<int, int> imageRowsRange(4, 4);
	std::pair<int, int> imageColsRange(4, 4);
	std::pair<int, int> imageGroupsRange(1, 1);
	std::pair<double, double> imageRelativeWidthRange(0.7, 0.7);
	std::pair<double, double> imageRelativeHeightRange(0.7, 0.7);


	if (ceil((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W) - (imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W < 0.01)
		num_W = ceil((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W);
	else
		num_W = floor((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W);

	if (ceil((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H) - (imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H < 0.01)
		num_H = ceil((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H);
	else
		num_H = floor((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H);
	/*std::cout << "num_W is " << num_W << std::endl;
	std::cout << "num_H is " << num_H << std::endl;*/
	cv::Scalar bg_color(0, 0, 0); // white back ground
	cv::Scalar window_color(255, 255, 255); // black for windows
	int thickness = -1;
	bool bSideW = false;
	bool bMidW = false;
	double ratioWidth = 0.0;
	double ratioHeight = 0.0;
	double ratioSideWidth = 0.0;
	double ratioSideHeight = 0.0;
	double ratioMidWidth = 0.0;
	double ratioMidHeight = 0.0;
	for (int row = imageRowsRange.first; row <= imageRowsRange.second; row++){ // loop row
		for (int col = imageColsRange.first; col <= imageColsRange.second; col++){ // loop col
			for (int relativeW = 0; relativeW <= num_W; relativeW++){ // loop relativeWidth
				for (int relativeH = 0; relativeH <= num_H; relativeH++){
					for (int relativeSideW = 0; relativeSideW <= num_W; relativeSideW++){ // loop relativeWidth
						for (int relativeMidW = 0; relativeMidW <= num_W; relativeMidW++){ // loop relativeWidth
							if (bSideW && bMidW){
								if (relativeW == relativeSideW || relativeW == relativeMidW)
									continue;
								if (col % 2 == 0)
									continue;
							}
							else if (bSideW && !bMidW){
								if (relativeW == relativeSideW || relativeW != relativeMidW)
									continue;
							}
							else if (!bSideW && bMidW){
								if (relativeW != relativeSideW || relativeW == relativeMidW)
									continue;
								if (col % 2 == 0)
									continue;
							}
							else{
								if (relativeW != relativeSideW || relativeW != relativeMidW)
									continue;
							}
							ratioWidth = relativeW * step_W + imageRelativeWidthRange.first;
							ratioHeight = relativeH * step_H + imageRelativeHeightRange.first;
							ratioSideWidth = relativeSideW * step_W + imageRelativeWidthRange.first;
							ratioSideHeight = relativeH * step_H + imageRelativeHeightRange.first;
							ratioMidWidth = relativeMidW * step_W + imageRelativeWidthRange.first;
							ratioMidHeight = relativeH * step_H + imageRelativeHeightRange.first;

							std::pair<double, double> imageRelativeW(ratioWidth, ratioHeight);
							std::pair<double, double> imageRelativeSideW(ratioSideWidth, ratioSideHeight);
							std::pair<double, double> imageRelativeMidW(ratioMidWidth, ratioMidHeight);
							int num_iters = 10;
							if (row >= 6 && col >= 6)
								num_iters = 30;
							if (int(window_prob) == 1)
								num_iters = 1;
							// draw facade image
							int num_deform = 3;
							if (int(1 - window_displacement) == 1)
								num_deform = 1;
							// draw facade image
							for (int iter_outers = 0; iter_outers < num_deform * num_iters; ++iter_outers){
								std::vector<cv::Mat> outpus_A = generateDeformFacade(width, height, row, col, 1, imageRelativeW, imageRelativeSideW, imageRelativeMidW, 0.1, window_prob, padding);
								// A
								QString img_filename_A = facadeImagesPath + QString("/A/facade_%1.png").arg(index, 5, 10, QChar('0'));
								//std::cout << "img_filename_A is " << img_filename_A.toUtf8().constData() << std::endl;
								cv::imwrite(img_filename_A.toUtf8().constData(), outpus_A[0]);
								// A_G
								QString img_filename_A_G = facadeImagesPath + QString("/A_G/facade_%1.png").arg(index, 5, 10, QChar('0'));
								//std::cout << "img_filename_G is " << img_filename_A_G.toUtf8().constData() << std::endl;
								cv::imwrite(img_filename_A_G.toUtf8().constData(), outpus_A[1]);

								std::vector<cv::Mat> outpus_B = generateDeformFacade(width, height, row, col, 1, imageRelativeW, imageRelativeSideW, imageRelativeMidW, 0.2, window_prob, padding);
								// B
								QString img_filename_B = facadeImagesPath + QString("/B/facade_%1.png").arg(index, 5, 10, QChar('0'));
								//std::cout << "img_filename_A is " << img_filename_A.toUtf8().constData() << std::endl;
								cv::imwrite(img_filename_B.toUtf8().constData(), outpus_B[0]);
								// B_G
								QString img_filename_B_G = facadeImagesPath + QString("/B_G/facade_%1.png").arg(index, 5, 10, QChar('0'));
								//std::cout << "img_filename_G is " << img_filename_B_G.toUtf8().constData() << std::endl;
								cv::imwrite(img_filename_B_G.toUtf8().constData(), outpus_B[1]);

								// fuse image
								cv::Mat fuse_img = fuse_images(outpus_A[1], outpus_B[1]);
								QString img_filename_G = facadeImagesPath + QString("/G/facade_%1.png").arg(index, 5, 10, QChar('0'));
								//std::cout << "img_filename_G is " << img_filename_G.toUtf8().constData() << std::endl;
								cv::imwrite(img_filename_G.toUtf8().constData(), fuse_img);

								// Eval image
								cv::Mat result_eval = generateFacade(width, height, row, col, 1, imageRelativeW, imageRelativeSideW, imageRelativeMidW, 0.0, 1.0, padding);
								QString img_filename_Eval = facadeImagesPath + QString("/Eval/facade_%1.png").arg(index, 5, 10, QChar('0'));
								//std::cout << "img_filename_Eval is " << img_filename_Eval.toUtf8().constData() << std::endl;
								cv::imwrite(img_filename_Eval.toUtf8().constData(), result_eval);

								index++;
							}
						}
					}
				}
			}
		}
	}
	return index;
}

int GLWidget3D::generateFuseDeformImages(QString facadeImagesPath, int index, int width, int height, float window_displacement, float window_prob, int padding){
	// generate facade images
	double step_W = 0.1;
	double step_H = 0.1;
	int num_W = 0;
	int num_H = 0;
	std::pair<int, int> imageRowsRange(2, 8);
	std::pair<int, int> imageColsRange(4, 8);
	std::pair<int, int> imageGroupsRange(1, 1);
	std::pair<double, double> imageRelativeWidthRange(0.2, 0.8);
	std::pair<double, double> imageRelativeHeightRange(0.2, 0.8);

	if (ceil((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W) - (imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W < 0.01)
		num_W = ceil((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W);
	else
		num_W = floor((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W);

	if (ceil((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H) - (imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H < 0.01)
		num_H = ceil((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H);
	else
		num_H = floor((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H);
	cv::Scalar bg_color(0, 0, 0); // white back ground
	cv::Scalar window_color(255, 255, 255); // black for windows
	int thickness = -1;
	double ratioWidth = 0.0;
	double ratioHeight = 0.0;
	for (int row = imageRowsRange.first; row <= imageRowsRange.second; row++){ // loop row
		for (int col = imageColsRange.first; col <= imageColsRange.second; col++){ // loop col
			int num_iters = 100;
			if (row >= 6 && col >= 6)
				num_iters = 100;
			num_iters = 10;
			int num_displacement = 10;
			int num_missing = 10;
			if (int(window_prob) == 1)
				num_missing = 1;
			if (int(1 - window_displacement) == 1)
				num_displacement = 1;

			for (int iter_outers = 0; iter_outers < num_iters * num_displacement * num_missing; ++iter_outers){
				cv::Mat result_A(height, width, CV_8UC1, bg_color);
				cv::Mat result_A_G(height, width, CV_8UC1, bg_color);
				cv::Mat result_B(height, width, CV_8UC1, bg_color);
				cv::Mat result_B_G(height, width, CV_8UC1, bg_color);
				cv::Mat result_G(height, width, CV_8UC1, bg_color);

				if (padding > 0){
					int top = padding;
					int bottom = padding;
					int left = padding;
					int right = padding;
					int borderType = cv::BORDER_CONSTANT;
					cv::copyMakeBorder(result_A, result_A, top, bottom, left, right, borderType, bg_color);
					cv::copyMakeBorder(result_A_G, result_A_G, top, bottom, left, right, borderType, bg_color);
					cv::copyMakeBorder(result_B, result_B, top, bottom, left, right, borderType, bg_color);
					cv::copyMakeBorder(result_B_G, result_B_G, top, bottom, left, right, borderType, bg_color);
					cv::copyMakeBorder(result_G, result_G, top, bottom, left, right, borderType, bg_color);
				}

				/* draw the facade */
				int NR = row;
				int NG = 1;
				int NC = col;
				double threshold = 0.05;
				double FW = width * 1.0 / NC;
				double FH = height * 1.0 / NR;
				std::vector<double> WW;
				std::vector<double> WH;
				// generate W and H list
				double AllW = 0.0;
				double AllH = 0.0;
				// compute the min start rw and wh
				int min_start_rw = ((threshold * width) / FW - imageRelativeWidthRange.first) / step_W;
				if (min_start_rw < 0)
					min_start_rw = 0;
				int min_start_rh = ((threshold * height) / FH - imageRelativeHeightRange.first) / step_H;
				if (min_start_rh < 0)
					min_start_rh = 0;
				// computer the min start rh
				int fixed_relativeW = util::genRand(min_start_rw, num_W + 1);
				int side_relativeW = util::genRand(min_start_rw, num_W + 1);
				for (int list = 0; list < col; list++){
					if (list == 0 || list == col - 1){
						WW.push_back(FW * (side_relativeW * step_W + imageRelativeWidthRange.first));
						AllW += FW * (side_relativeW * step_W + imageRelativeWidthRange.first);
					}
					else{
						WW.push_back(FW * (fixed_relativeW * step_W + imageRelativeWidthRange.first));
						AllW += FW * (fixed_relativeW * step_W + imageRelativeWidthRange.first);
					}
				}
				int fixed_relativeH = util::genRand(min_start_rh, num_H + 1);
				for (int list = 0; list < row; list++){
					WH.push_back(FH * (fixed_relativeH * step_H + imageRelativeHeightRange.first));
					AllH += FH * (fixed_relativeH * step_H + imageRelativeHeightRange.first);
				}

				double width_spacing = 0.0f;
				double height_spacing = 0.0f;
				if (NC > 1)
					width_spacing = (width - AllW) / (NC - 1);
				if (NR > 1)
					height_spacing = (height - AllH) / (NR - 1);


				// compute spacing adjustment
				// currently only adjust right side
				// e.g. between -0.5 * curW and -0.5 * width_spacing
				std::vector<double> SpacingW;
				std::vector<double> SpacingH;
				// side spacing
				float adjustmentW = util::genRand(-0.5 * width_spacing, 0.5 * width_spacing);
				float width_spacing_new = width_spacing + adjustmentW;
				if (col - 1 >= 3){
					width_spacing = (width - AllW - 2 * width_spacing_new) / (NC - 3);
				}
				for (int list = 0; list < col - 1; list++){
					if (col - 1 >= 3 && (list == 0 || list == col -2)){
						SpacingW.push_back(width_spacing_new);
					}
					else{
						SpacingW.push_back(width_spacing);
					}

				}
				for (int list = 0; list < row - 1; list++){
					SpacingH.push_back(height_spacing);
				}

				// check valid
				bool bValid_Window = true;
				for (int list = 0; list < WW.size(); list++){
					if (WW[list] < threshold * width){
						bValid_Window = false;
						break;
					}
				}
				for (int list = 0; list < WH.size(); list++){
					if (WH[list] < threshold * height){
						bValid_Window = false;
						break;
					}
				}

				// check valid of spacing
				bool bValid_Spacing = true;
				for (int list = 0; list < SpacingW.size(); list++){
					if (SpacingW[list] < threshold * width){
						bValid_Spacing = false;
						break;
					}
				}
				for (int list = 0; list < SpacingH.size(); list++){
					if (SpacingH[list] < threshold * height){
						bValid_Spacing = false;
						break;
					}
				}

				if (!bValid_Window || !bValid_Spacing){
					//std::cout << "Invalid image" << std::endl;
					continue;
				}

				if (NG == 1){
					float curH_spacing = 0;
					float curW_spacing = 0;
					for (int i = 0; i < NR; ++i) {
						curW_spacing = 0;
						for (int j = 0; j < NC; ++j) {
							float x1_A, y1_A, x2_A, y2_A;
							float x1_A_G, y1_A_G, x2_A_G, y2_A_G;
							float x1_B, y1_B, x2_B, y2_B;
							float x1_B_G, y1_B_G, x2_B_G, y2_B_G;
							float x1_G, y1_G, x2_G, y2_G;

							float curW = WW[j];
							float curH = WH[i];
							// A
							x1_A = curW_spacing;
							y1_A = curH_spacing;
							x2_A = x1_A + curW;
							y2_A = y1_A + curH;
							// A_G
							x1_A_G = curW_spacing;
							y1_A_G = curH_spacing;
							x2_A_G = x1_A_G + curW;
							y2_A_G = y1_A_G + curH;
							// B
							x1_B = curW_spacing;
							y1_B = curH_spacing;
							x2_B = x1_B + curW;
							y2_B = y1_B + curH;
							// B_G
							x1_B_G = curW_spacing;
							y1_B_G = curH_spacing;
							x2_B_G = x1_B_G + curW;
							y2_B_G = y1_B_G + curH;
							//G
							x1_G = curW_spacing;
							y1_G = curH_spacing;
							x2_G = x1_G + curW;
							y2_G = y1_G + curH;

							if (window_displacement > 0) {
								x1_A += util::genRand(-curW * window_displacement, curW * window_displacement);
								y1_A += util::genRand(-curH * window_displacement, curH * window_displacement);
								x2_A += util::genRand(-curW * window_displacement, curW * window_displacement);
								y2_A += util::genRand(-curH * window_displacement, curH * window_displacement);

								// can not be too small
								if (x2_A - x1_A < threshold * width)
								{
									x1_A = curW_spacing;
									x2_A = x1_A + curW;
								}
								if (y2_A - y1_A < threshold * height){
									y1_A = curH_spacing;
									y2_A = y1_A + curH;
								}

								x1_B += util::genRand(-curW * window_displacement, curW * window_displacement);
								y1_B += util::genRand(-curH * window_displacement, curH * window_displacement);
								x2_B += util::genRand(-curW * window_displacement, curW * window_displacement);
								y2_B += util::genRand(-curH * window_displacement, curH * window_displacement);

								// can not be too small
								if (x2_B - x1_B < threshold * width)
								{
									x1_B = curW_spacing;
									x2_B = x1_B + curW;
								}
								if (y2_B - y1_B < threshold * height){
									y1_B = curH_spacing;
									y2_B = y1_B + curH;
								}

							}

							if (util::genRand() < window_prob) {
								if (padding > 0){
									cv::rectangle(result_A, cv::Point(std::round(x1_A) + padding, std::round(y1_A) + padding), cv::Point(std::round(x2_A) + padding, std::round(y2_A) + padding), window_color, thickness);
									cv::rectangle(result_A_G, cv::Point(std::round(x1_A_G) + padding, std::round(y1_A_G) + padding), cv::Point(std::round(x2_A_G) + padding, std::round(y2_A_G) + padding), window_color, thickness);
								}
								else{
									cv::rectangle(result_A, cv::Point(std::round(x1_A), std::round(y1_A)), cv::Point(std::round(x2_A), std::round(y2_A)), window_color, thickness);
									cv::rectangle(result_A_G, cv::Point(std::round(x1_A_G), std::round(y1_A_G)), cv::Point(std::round(x2_A_G), std::round(y2_A_G)), window_color, thickness);
								}
							}

							if (util::genRand() < window_prob) {
								if (padding > 0){
									cv::rectangle(result_B, cv::Point(std::round(x1_B) + padding, std::round(y1_B) + padding), cv::Point(std::round(x2_B) + padding, std::round(y2_B) + padding), window_color, thickness);
									cv::rectangle(result_B_G, cv::Point(std::round(x1_B_G) + padding, std::round(y1_B_G) + padding), cv::Point(std::round(x2_B_G) + padding, std::round(y2_B_G) + padding), window_color, thickness);
								}
								else{
									cv::rectangle(result_B, cv::Point(std::round(x1_B), std::round(y1_B)), cv::Point(std::round(x2_B), std::round(y2_B)), window_color, thickness);
									cv::rectangle(result_B_G, cv::Point(std::round(x1_B_G), std::round(y1_B_G)), cv::Point(std::round(x2_B_G), std::round(y2_B_G)), window_color, thickness);
								}
							}
							cv::rectangle(result_G, cv::Point(std::round(x1_G) + padding, std::round(y1_G) + padding), cv::Point(std::round(x2_G) + padding, std::round(y2_G) + padding), window_color, thickness);
							if (j < NC - 1)
								curW_spacing += WW[j] + SpacingW[j];
						}
						if (i < NR - 1)
							curH_spacing += SpacingH[i] + WH[i];
					}
				}
				// A
				QString img_filename_A = facadeImagesPath + QString("/A/facade_%1.png").arg(index, 5, 10, QChar('0'));
				cv::imwrite(img_filename_A.toUtf8().constData(), result_A);
				// A_G
				QString img_filename_A_G = facadeImagesPath + QString("/A_G/facade_%1.png").arg(index, 5, 10, QChar('0'));
				cv::imwrite(img_filename_A_G.toUtf8().constData(), result_A_G);
				// B
				QString img_filename_B = facadeImagesPath + QString("/B/facade_%1.png").arg(index, 5, 10, QChar('0'));
				cv::imwrite(img_filename_B.toUtf8().constData(), result_B);
				// B_G
				QString img_filename_B_G = facadeImagesPath + QString("/B_G/facade_%1.png").arg(index, 5, 10, QChar('0'));
				cv::imwrite(img_filename_B_G.toUtf8().constData(), result_B_G);

				// fuse image
				cv::Mat fuse_img = fuse_images(result_A_G, result_B_G);
				QString img_filename_G = facadeImagesPath + QString("/G/facade_%1.png").arg(index, 5, 10, QChar('0'));
				cv::imwrite(img_filename_G.toUtf8().constData(), fuse_img);

				// Eval image
				QString img_filename_Eval = facadeImagesPath + QString("/Eval/facade_%1.png").arg(index, 5, 10, QChar('0'));
				cv::imwrite(img_filename_Eval.toUtf8().constData(), result_G);

				//std::cout << img_filename_A.toUtf8().constData() << std::endl;
				index++;

			}
		}
	}
	return index;
}

//int GLWidget3D::generateFuseDeformImages(QString facadeImagesPath, int index, int width, int height, float window_displacement, float window_prob, int padding){
//	// generate facade images
//	double step_W = 0.1;
//	double step_H = 0.1;
//	int num_W = 0;
//	int num_H = 0;
//	std::pair<int, int> imageRowsRange(2, 8);
//	std::pair<int, int> imageColsRange(2, 8);
//	std::pair<int, int> imageGroupsRange(1, 1);
//	std::pair<double, double> imageRelativeWidthRange(0.2, 0.8);
//	std::pair<double, double> imageRelativeHeightRange(0.2, 0.8);
//
//
//	if (ceil((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W) - (imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W < 0.01)
//		num_W = ceil((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W);
//	else
//		num_W = floor((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W);
//
//	if (ceil((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H) - (imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H < 0.01)
//		num_H = ceil((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H);
//	else
//		num_H = floor((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H);
//	/*std::cout << "num_W is " << num_W << std::endl;
//	std::cout << "num_H is " << num_H << std::endl;*/
//	cv::Scalar bg_color(0, 0, 0); // white back ground
//	cv::Scalar window_color(255, 255, 255); // black for windows
//	int thickness = -1;
//	bool bSideW = false;
//	bool bMidW = false;
//	double ratioWidth = 0.0;
//	double ratioHeight = 0.0;
//	double ratioSideWidth = 0.0;
//	double ratioSideHeight = 0.0;
//	double ratioMidWidth = 0.0;
//	double ratioMidHeight = 0.0;
//	for (int row = imageRowsRange.first; row <= imageRowsRange.second; row++){ // loop row
//		for (int col = imageColsRange.first; col <= imageColsRange.second; col++){ // loop col
//			for (int relativeW = 0; relativeW <= num_W; relativeW++){ // loop relativeWidth
//				for (int relativeH = 0; relativeH <= num_H; relativeH++){
//					for (int relativeSideW = 0; relativeSideW <= num_W; relativeSideW++){ // loop relativeWidth
//						for (int relativeMidW = 0; relativeMidW <= num_W; relativeMidW++){ // loop relativeWidth
//							if (bSideW && bMidW){
//								if (relativeW == relativeSideW || relativeW == relativeMidW)
//									continue;
//								if (col % 2 == 0)
//									continue;
//							}
//							else if (bSideW && !bMidW){
//								if (relativeW == relativeSideW || relativeW != relativeMidW)
//									continue;
//							}
//							else if (!bSideW && bMidW){
//								if (relativeW != relativeSideW || relativeW == relativeMidW)
//									continue;
//								if (col % 2 == 0)
//									continue;
//							}
//							else{
//								if (relativeW != relativeSideW || relativeW != relativeMidW)
//									continue;
//							}
//							ratioWidth = relativeW * step_W + imageRelativeWidthRange.first;
//							ratioHeight = relativeH * step_H + imageRelativeHeightRange.first;
//							ratioSideWidth = relativeSideW * step_W + imageRelativeWidthRange.first;
//							ratioSideHeight = relativeH * step_H + imageRelativeHeightRange.first;
//							ratioMidWidth = relativeMidW * step_W + imageRelativeWidthRange.first;
//							ratioMidHeight = relativeH * step_H + imageRelativeHeightRange.first;
//
//							std::pair<double, double> imageRelativeW(ratioWidth, ratioHeight);
//							std::pair<double, double> imageRelativeSideW(ratioSideWidth, ratioSideHeight);
//							std::pair<double, double> imageRelativeMidW(ratioMidWidth, ratioMidHeight);
//							int num_iters = 10;
//							if (row >= 6 && col >= 6)
//								num_iters = 30;
//							num_iters = 2;
//							if (int(window_prob) == 1)
//								num_iters = 1;
//							// draw facade image
//							int num_deform = 2;
//							if (int(1 - window_displacement) == 1)
//								num_deform = 1;
//							// draw facade image
//							for (int iter_outers = 0; iter_outers < num_deform * num_iters; ++iter_outers){
//								std::vector<cv::Mat> outpus_A = generateDeformFacade(width, height, row, col, 1, imageRelativeW, imageRelativeSideW, imageRelativeMidW, 0.1, window_prob, padding);
//								if (outpus_A.size() == 0){
//									//std::cout << "invlaid image" << std::endl;
//									continue;
//								}
//								std::vector<cv::Mat> outpus_B = generateDeformFacade(width, height, row, col, 1, imageRelativeW, imageRelativeSideW, imageRelativeMidW, 0.2, window_prob, padding);
//								if (outpus_B.size() == 0){
//									//std::cout << "invlaid image" << std::endl;
//									continue;
//								}
//								// A
//								QString img_filename_A = facadeImagesPath + QString("/A/facade_%1.png").arg(index, 5, 10, QChar('0'));
//								//std::cout << "img_filename_A is " << img_filename_A.toUtf8().constData() << std::endl;
//								cv::imwrite(img_filename_A.toUtf8().constData(), outpus_A[0]);
//								// A_G
//								QString img_filename_A_G = facadeImagesPath + QString("/A_G/facade_%1.png").arg(index, 5, 10, QChar('0'));
//								//std::cout << "img_filename_G is " << img_filename_A_G.toUtf8().constData() << std::endl;
//								cv::imwrite(img_filename_A_G.toUtf8().constData(), outpus_A[1]);
//
//								
//								// B
//								QString img_filename_B = facadeImagesPath + QString("/B/facade_%1.png").arg(index, 5, 10, QChar('0'));
//								//std::cout << "img_filename_A is " << img_filename_A.toUtf8().constData() << std::endl;
//								cv::imwrite(img_filename_B.toUtf8().constData(), outpus_B[0]);
//								// B_G
//								QString img_filename_B_G = facadeImagesPath + QString("/B_G/facade_%1.png").arg(index, 5, 10, QChar('0'));
//								//std::cout << "img_filename_G is " << img_filename_B_G.toUtf8().constData() << std::endl;
//								cv::imwrite(img_filename_B_G.toUtf8().constData(), outpus_B[1]);
//
//								// fuse image
//								cv::Mat fuse_img = fuse_images(outpus_A[1], outpus_B[1]);
//								QString img_filename_G = facadeImagesPath + QString("/G/facade_%1.png").arg(index, 5, 10, QChar('0'));
//								//std::cout << "img_filename_G is " << img_filename_G.toUtf8().constData() << std::endl;
//								cv::imwrite(img_filename_G.toUtf8().constData(), fuse_img);
//
//								// Eval image
//								cv::Mat result_eval = generateFacade(width, height, row, col, 1, imageRelativeW, imageRelativeSideW, imageRelativeMidW, 0.0, 1.0, padding);
//								QString img_filename_Eval = facadeImagesPath + QString("/Eval/facade_%1.png").arg(index, 5, 10, QChar('0'));
//								//std::cout << "img_filename_Eval is " << img_filename_Eval.toUtf8().constData() << std::endl;
//								cv::imwrite(img_filename_Eval.toUtf8().constData(), result_eval);
//
//								index++;
//							}
//						}
//					}
//				}
//			}
//		}
//	}
//	return index;
//}


int GLWidget3D::generateScoreImages(QString facadeImagesPath, int index, int width, int height, float window_displacement, float window_prob, int padding){
	// generate facade images
	double step_W = 0.2;
	double step_H = 0.2;
	int num_W = 0;
	int num_H = 0;
	std::pair<int, int> imageRowsRange(3, 8);
	std::pair<int, int> imageColsRange(3, 8);

	std::pair<int, int> imageGroupsRange(1, 1);
	std::pair<double, double> imageRelativeWidthRange(0.3, 0.7);
	//std::pair<double, double> imageRelativeWidthRange(0.2, 0.7);
	std::pair<double, double> imageRelativeHeightRange(0.3, 0.7);

	if (ceil((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W) - (imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W < 0.01)
		num_W = ceil((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W);
	else
		num_W = floor((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W);

	if (ceil((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H) - (imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H < 0.01)
		num_H = ceil((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H);
	else
		num_H = floor((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H);
	//std::cout << "num_W is " << num_W << std::endl;
	//std::cout << "num_H is " << num_H << std::endl;
	cv::Scalar bg_color(0, 0, 0); // white back ground
	cv::Scalar window_color(255, 255, 255); // black for windows
	int thickness = -1;
	double ratioWidth = 0.0;
	double ratioHeight = 0.0;
	for (int row = imageRowsRange.first; row <= imageRowsRange.second; row++){ // loop row
		for (int col = imageColsRange.first; col <= imageColsRange.second; col++){ // loop col
			int num_iters = 50;// 50
			if (row >= 6 && col >= 6)
				num_iters = 70; // 70
			for (int iter = 0; iter < 100/*num_iters*/; iter++){
				/* draw the facade */
				int NR = row;
				int NG = 1;
				int NC = col;
				double FW = width * 1.0 / NC;
				double FH = height * 1.0 / NR;
				std::vector<double> WW;
				std::vector<double> WH;
				// generate W and H list
				double AllW = 0.0;
				double AllH = 0.0;
				
				for (int list = 0; list < col; list++){
					int relativeW = util::genRand(0, num_W + 1);
					WW.push_back(FW * (relativeW * step_W + imageRelativeWidthRange.first));
					AllW += FW * (relativeW * step_W + imageRelativeWidthRange.first);
				}
				for (int list = 0; list < row; list++){
					int relativeH = util::genRand(0, num_H + 1);
					WH.push_back(FH * (relativeH * step_H + imageRelativeHeightRange.first));
					AllH += FH * (relativeH * step_H + imageRelativeHeightRange.first);
				}
				double width_spacing = 0.0f;
				double height_spacing = 0.0f;
				if (NC > 1)
					width_spacing = (width - AllW) / (NC - 1);
				if (NR > 1)
					height_spacing = (height - AllH) / (NR - 1);

				// check valid
				double threshold = 0.05;
				bool bValid_Window = true;
				bool bValid_Spacing = true;
				for (int list = 0; list < WW.size(); list++){
					if (WW[list] <= threshold * width){
						bValid_Window = false;
						break;
					}
				}
				for (int list = 0; list < WH.size(); list++){
					if (WH[list] <= threshold * height){
						bValid_Window = false;
						break;
					}
				}
				if (!bValid_Window || width_spacing <= threshold * width || height_spacing <= threshold * height){
					//std::cout << "Invalid image" << std::endl;
					continue;
				}

				int num_displacement = 10;
				int num_missing = 10;
				if (int(window_prob) == 1)
					num_missing = 1;
				if (int(1 - window_displacement) == 1)
					num_displacement = 1;

				for (int iter_outers = 0; iter_outers < num_displacement * num_missing; ++iter_outers){
					cv::Mat result(height, width, CV_8UC1, bg_color);
					cv::Mat result_G(height, width, CV_8UC1, bg_color);
					if (padding > 0){
						int top = padding;
						int bottom = padding;
						int left = padding;
						int right = padding;
						int borderType = cv::BORDER_CONSTANT;
						cv::copyMakeBorder(result, result, top, bottom, left, right, borderType, bg_color);
						cv::copyMakeBorder(result_G, result_G, top, bottom, left, right, borderType, bg_color);
					}

					if (NG == 1){
						float curH_spacing = 0;
						float curW_spacing = 0;
						for (int i = 0; i < NR; ++i) {
							curW_spacing = 0;
							for (int j = 0; j < NC; ++j) {
								float x1, y1, x2, y2;
								float x1_G, y1_G, x2_G, y2_G;
								float curW = WW[j];
								float curH = WH[i];

								x1 = curW_spacing;
								y1 = curH_spacing;
								x2 = x1 + curW;
								y2 = y1 + curH;
								// G
								x1_G = curW_spacing;
								y1_G = curH_spacing;
								x2_G = x1_G + curW;
								y2_G = y1_G + curH;
								curW_spacing += curW + width_spacing;
								if (window_displacement > 0) {
									x1 += util::genRand(-curW * window_displacement, curW * window_displacement);
									y1 += util::genRand(-curH * window_displacement, curH * window_displacement);
									x2 += util::genRand(-curW * window_displacement, curW * window_displacement);
									y2 += util::genRand(-curH * window_displacement, curH * window_displacement);
									/*x1 += util::genRand(0, curW * window_displacement);
									y1 += util::genRand(0, curH * window_displacement);
									x2 += util::genRand(-curW * window_displacement, 0);
									y2 += util::genRand(-curH * window_displacement, 0);*/
								}

								if (util::genRand() < window_prob) {
									if (padding > 0){
										cv::rectangle(result, cv::Point(std::round(x1) + padding, std::round(y1) + padding), cv::Point(std::round(x2) + padding, std::round(y2) + padding), window_color, thickness);
										cv::rectangle(result_G, cv::Point(std::round(x1_G) + padding, std::round(y1_G) + padding), cv::Point(std::round(x2_G) + padding, std::round(y2_G) + padding), window_color, thickness);
									}
									else{
										cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), window_color, thickness);
										cv::rectangle(result_G, cv::Point(std::round(x1_G), std::round(y1_G)), cv::Point(std::round(x2_G), std::round(y2_G)), window_color, thickness);
									}
								}
							}
							curH_spacing += height_spacing + WH[i];
						}
					}
					int padding_new = 4;
					if (padding_new > 0){
						int top = padding_new;
						int bottom = padding_new;
						int left = padding_new;
						int right = padding_new;
						int borderType = cv::BORDER_CONSTANT;
						cv::copyMakeBorder(result, result, top, bottom, left, right, borderType, bg_color);
						cv::copyMakeBorder(result_G, result_G, top, bottom, left, right, borderType, bg_color);
					}
					// A
					QString img_filename_A = facadeImagesPath + QString("/A/facade_%1.png").arg(index, 5, 10, QChar('0'));
					//std::cout << "img_filename_A is " << img_filename_A.toUtf8().constData() << std::endl;
					cv::imwrite(img_filename_A.toUtf8().constData(), result);
					// G
					QString img_filename_G = facadeImagesPath + QString("/G/facade_%1.png").arg(index, 5, 10, QChar('0'));
					//std::cout << "img_filename_G is " << img_filename_G.toUtf8().constData() << std::endl;
					cv::imwrite(img_filename_G.toUtf8().constData(), result_G);
					index++;
				}

			}
		}
	}
	return index;
}


void GLWidget3D::countScoreImages(QString facadeImagesPath, int index, int width, int height, int row, int col, float window_displacement, float window_prob, int padding){
	// generate facade images
	double step_W = 0.2;
	double step_H = 0.2;
	int num_W = 0;
	int num_H = 0;
	std::pair<int, int> imageGroupsRange(1, 1);
	//std::pair<double, double> imageRelativeWidthRange(0.2, 0.8);
	//std::pair<double, double> imageRelativeHeightRange(0.3, 0.8);
	std::pair<double, double> imageRelativeWidthRange(0.3, 0.7);
	std::pair<double, double> imageRelativeHeightRange(0.3, 0.7);

	if (ceil((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W) - (imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W < 0.01)
		num_W = ceil((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W);
	else
		num_W = floor((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W);

	if (ceil((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H) - (imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H < 0.01)
		num_H = ceil((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H);
	else
		num_H = floor((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H);
	//std::cout << "num_W is " << num_W << std::endl;
	//std::cout << "num_H is " << num_H << std::endl;
	cv::Scalar bg_color(0, 0, 0); // white back ground
	cv::Scalar window_color(255, 255, 255); // black for windows
	int thickness = -1;
	double ratioWidth = 0.0;
	double ratioHeight = 0.0;
	
	/* draw the facade */
	int NR = row;
	int NG = 1;
	int NC = col;
	double FW = width * 1.0 / NC;
	double FH = height * 1.0 / NR;
	std::vector<double> WW;
	std::vector<double> WH;
	// generate W and H list
	double AllW = 0.0;
	double AllH = 0.0;
	for (int list = 0; list < col; list++){
		int relativeW = 2;
		WW.push_back(FW * (relativeW * step_W + imageRelativeWidthRange.first));
		AllW += FW * (relativeW * step_W + imageRelativeWidthRange.first);
	}
	for (int list = 0; list < row; list++){
		int relativeH = 2;
		WH.push_back(FH * (relativeH * step_H + imageRelativeHeightRange.first));
		AllH += FH * (relativeH * step_H + imageRelativeHeightRange.first);
	}
	double width_spacing = 0.0f;
	double height_spacing = 0.0f;
	if (NC > 1)
		width_spacing = (width - AllW) / (NC - 1);
	if (NR > 1)
		height_spacing = (height - AllH) / (NR - 1);


	cv::Mat result(height, width, CV_8UC1, bg_color);
	cv::Mat result_G(height, width, CV_8UC1, bg_color);
	if (padding > 0){
		int top = padding;
		int bottom = padding;
		int left = padding;
		int right = padding;
		int borderType = cv::BORDER_CONSTANT;
		cv::copyMakeBorder(result, result, top, bottom, left, right, borderType, bg_color);
		cv::copyMakeBorder(result_G, result_G, top, bottom, left, right, borderType, bg_color);
	}

	if (NG == 1){
		float curH_spacing = 0;
		float curW_spacing = 0;
		for (int i = 0; i < NR; ++i) {
			curW_spacing = 0;
			for (int j = 0; j < NC; ++j) {
				float x1, y1, x2, y2;
				float x1_G, y1_G, x2_G, y2_G;
				float curW = WW[j];
				float curH = WH[i];

				x1 = curW_spacing;
				y1 = curH_spacing;
				x2 = x1 + curW;
				y2 = y1 + curH;
				// G
				x1_G = curW_spacing;
				y1_G = curH_spacing;
				x2_G = x1_G + curW;
				y2_G = y1_G + curH;
				curW_spacing += curW + width_spacing;
				if (window_displacement > 0) {
					x1 += util::genRand(-curW * window_displacement, curW * window_displacement);
					y1 += util::genRand(-curH * window_displacement, curH * window_displacement);
					x2 += util::genRand(-curW * window_displacement, curW * window_displacement);
					y2 += util::genRand(-curH * window_displacement, curH * window_displacement);
				}

				if (util::genRand() < window_prob) {
					if (padding > 0){
						cv::rectangle(result, cv::Point(std::round(x1) + padding, std::round(y1) + padding), cv::Point(std::round(x2) + padding, std::round(y2) + padding), window_color, thickness);
						cv::rectangle(result_G, cv::Point(std::round(x1_G) + padding, std::round(y1_G) + padding), cv::Point(std::round(x2_G) + padding, std::round(y2_G) + padding), window_color, thickness);
					}
					else{
						cv::rectangle(result, cv::Point(std::round(x1), std::round(y1)), cv::Point(std::round(x2), std::round(y2)), window_color, thickness);
						cv::rectangle(result_G, cv::Point(std::round(x1_G), std::round(y1_G)), cv::Point(std::round(x2_G), std::round(y2_G)), window_color, thickness);
					}
				}
			}
			curH_spacing += height_spacing + WH[i];
		}
	}
	// A
	QString img_filename_A = facadeImagesPath + QString("/A/facade_%1.png").arg(index, 5, 10, QChar('0'));
	//std::cout << "img_filename_A is " << img_filename_A.toUtf8().constData() << std::endl;
	cv::imwrite(img_filename_A.toUtf8().constData(), result);
	// G
	QString img_filename_G = facadeImagesPath + QString("/G/facade_%1.png").arg(index, 5, 10, QChar('0'));
	//std::cout << "img_filename_G is " << img_filename_G.toUtf8().constData() << std::endl;
	cv::imwrite(img_filename_G.toUtf8().constData(), result_G);
}


//int GLWidget3D::generateScoreFuseImages(QString facadeImagesPath, int index, int width, int height, float window_displacement, float window_prob, int padding){
//	// generate facade images
//	double step_W = 0.2;
//	double step_H = 0.2;
//	int num_W = 0;
//	int num_H = 0;
//	std::pair<int, int> imageRowsRange(3, 8);
//	std::pair<int, int> imageColsRange(3, 8);
//	std::pair<int, int> imageGroupsRange(1, 1);
//	std::pair<double, double> imageRelativeWidthRange(0.3, 0.7);
//	std::pair<double, double> imageRelativeHeightRange(0.3, 0.7);
//
//	if (ceil((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W) - (imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W < 0.01)
//		num_W = ceil((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W);
//	else
//		num_W = floor((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W);
//
//	if (ceil((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H) - (imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H < 0.01)
//		num_H = ceil((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H);
//	else
//		num_H = floor((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H);
//	//std::cout << "num_W is " << num_W << std::endl;
//	//std::cout << "num_H is " << num_H << std::endl;
//	cv::Scalar bg_color(0, 0, 0); // white back ground
//	cv::Scalar window_color(255, 255, 255); // black for windows
//	int thickness = -1;
//	double ratioWidth = 0.0;
//	double ratioHeight = 0.0;
//	for (int row = imageRowsRange.first; row <= imageRowsRange.second; row++){ // loop row
//		for (int col = imageColsRange.first; col <= imageColsRange.second; col++){ // loop col
//			window_prob = util::genRand(0.7, 1.0);
//			window_displacement = util::genRand(0.0, 0.2);
//			int num_iters = 15;
//			if (row >= 6 && col >= 6)
//				num_iters = 30;
//			int num_displacement = 10;
//			int num_missing = 10;
//			if (int(window_prob) == 1)
//				num_missing = 1;
//			if (int(1 - window_displacement) == 1)
//				num_displacement = 1;
//
//			for (int iter_outers = 0; iter_outers < 2 * num_iters * num_displacement * num_missing; ++iter_outers){
//				cv::Mat result_A(height, width, CV_8UC1, bg_color);
//				cv::Mat result_A_G(height, width, CV_8UC1, bg_color);
//				cv::Mat result_B(height, width, CV_8UC1, bg_color);
//				cv::Mat result_B_G(height, width, CV_8UC1, bg_color);
//				cv::Mat result_G(height, width, CV_8UC1, bg_color);
//				/* draw the facade */
//				int NR = row;
//				int NG = 1;
//				int NC = col;
//				double FW = width * 1.0 / NC;
//				double FH = height * 1.0 / NR;
//				std::vector<double> WW;
//				std::vector<double> WH;
//				// generate W and H list
//				double AllW = 0.0;
//				double AllH = 0.0;
//				for (int list = 0; list < col; list++){
//					int relativeW = util::genRand(0, num_W + 1);
//					WW.push_back(FW * (relativeW * step_W + imageRelativeWidthRange.first));
//					AllW += FW * (relativeW * step_W + imageRelativeWidthRange.first);
//				}
//				for (int list = 0; list < row; list++){
//					int relativeH = util::genRand(0, num_H + 1);
//					WH.push_back(FH * (relativeH * step_H + imageRelativeHeightRange.first));
//					AllH += FH * (relativeH * step_H + imageRelativeHeightRange.first);
//				}
//
//				double width_spacing = 0.0f;
//				double height_spacing = 0.0f;
//				if (NC > 2)
//					width_spacing = (width - AllW) / (NC - 1);
//				if (NR > 1)
//					height_spacing = (height - AllH) / (NR - 1);
//
//				if (NG == 1){
//					float curH_spacing = 0;
//					float curW_spacing = 0;
//					for (int i = 0; i < NR; ++i) {
//						curW_spacing = 0;
//						for (int j = 0; j < NC; ++j) {
//							float x1_A, y1_A, x2_A, y2_A;
//							float x1_A_G, y1_A_G, x2_A_G, y2_A_G;
//							float x1_B, y1_B, x2_B, y2_B;
//							float x1_B_G, y1_B_G, x2_B_G, y2_B_G;
//							float x1_G, y1_G, x2_G, y2_G;
//
//							float curW = WW[j];
//							float curH = WH[i];
//
//							// A
//							x1_A = curW_spacing;
//							y1_A = curH_spacing;
//							x2_A = x1_A + curW;
//							y2_A = y1_A + curH;
//							// A_G
//							x1_A_G = curW_spacing;
//							y1_A_G = curH_spacing;
//							x2_A_G = x1_A_G + curW;
//							y2_A_G = y1_A_G + curH;
//							// B
//							x1_B = curW_spacing;
//							y1_B = curH_spacing;
//							x2_B = x1_B + curW;
//							y2_B = y1_B + curH;
//							// B_G
//							x1_B_G = curW_spacing;
//							y1_B_G = curH_spacing;
//							x2_B_G = x1_B_G + curW;
//							y2_B_G = y1_B_G + curH;
//							//G
//							x1_G = curW_spacing;
//							y1_G = curH_spacing;
//							x2_G = x1_G + curW;
//							y2_G = y1_G + curH;
//
//							curW_spacing += curW + width_spacing;
//							if (window_displacement > 0) {
//								x1_A += util::genRand(-curW * window_displacement, curW * window_displacement);
//								y1_A += util::genRand(-curH * window_displacement, curH * window_displacement);
//								x2_A += util::genRand(-curW * window_displacement, curW * window_displacement);
//								y2_A += util::genRand(-curH * window_displacement, curH * window_displacement);
//
//								x1_B += util::genRand(-curW * window_displacement, curW * window_displacement);
//								y1_B += util::genRand(-curH * window_displacement, curH * window_displacement);
//								x2_B += util::genRand(-curW * window_displacement, curW * window_displacement);
//								y2_B += util::genRand(-curH * window_displacement, curH * window_displacement);
//
//							}
//
//							if (util::genRand() < window_prob) {
//								cv::rectangle(result_A, cv::Point(std::round(x1_A), std::round(y1_A)), cv::Point(std::round(x2_A), std::round(y2_A)), window_color, thickness);
//								cv::rectangle(result_A_G, cv::Point(std::round(x1_A_G), std::round(y1_A_G)), cv::Point(std::round(x2_A_G), std::round(y2_A_G)), window_color, thickness);
//							}
//
//							if (util::genRand() < window_prob) {
//								cv::rectangle(result_B, cv::Point(std::round(x1_B), std::round(y1_B)), cv::Point(std::round(x2_B), std::round(y2_B)), window_color, thickness);
//								cv::rectangle(result_B_G, cv::Point(std::round(x1_B_G), std::round(y1_B_G)), cv::Point(std::round(x2_B_G), std::round(y2_B_G)), window_color, thickness);
//							}
//							cv::rectangle(result_G, cv::Point(std::round(x1_G), std::round(y1_G)), cv::Point(std::round(x2_G), std::round(y2_G)), window_color, thickness);
//						}
//						curH_spacing += height_spacing + WH[i];
//					}
//				}
//				if (padding > 0){
//					int top = padding;
//					int bottom = padding;
//					int left = padding;
//					int right = padding;
//					int borderType = cv::BORDER_CONSTANT;
//					cv::copyMakeBorder(result_A, result_A, top, bottom, left, right, borderType, bg_color);
//					cv::copyMakeBorder(result_A_G, result_A_G, top, bottom, left, right, borderType, bg_color);
//					cv::copyMakeBorder(result_B, result_B, top, bottom, left, right, borderType, bg_color);
//					cv::copyMakeBorder(result_B_G, result_B_G, top, bottom, left, right, borderType, bg_color);
//					cv::copyMakeBorder(result_G, result_G, top, bottom, left, right, borderType, bg_color);
//				}
//				// A
//				QString img_filename_A = facadeImagesPath + QString("/A/facade_%1.png").arg(index, 5, 10, QChar('0'));
//				cv::imwrite(img_filename_A.toUtf8().constData(), result_A);
//				// A_G
//				QString img_filename_A_G = facadeImagesPath + QString("/A_G/facade_%1.png").arg(index, 5, 10, QChar('0'));
//				cv::imwrite(img_filename_A_G.toUtf8().constData(), result_A_G);
//				// B
//				QString img_filename_B = facadeImagesPath + QString("/B/facade_%1.png").arg(index, 5, 10, QChar('0'));
//				cv::imwrite(img_filename_B.toUtf8().constData(), result_B);
//				// B_G
//				QString img_filename_B_G = facadeImagesPath + QString("/B_G/facade_%1.png").arg(index, 5, 10, QChar('0'));
//				cv::imwrite(img_filename_B_G.toUtf8().constData(), result_B_G);
//
//				// fuse image
//				cv::Mat fuse_img = fuse_images(result_A_G, result_B_G);
//				QString img_filename_G = facadeImagesPath + QString("/G/facade_%1.png").arg(index, 5, 10, QChar('0'));
//				cv::imwrite(img_filename_G.toUtf8().constData(), fuse_img);
//
//				// Eval image
//				QString img_filename_Eval = facadeImagesPath + QString("/Eval/facade_%1.png").arg(index, 5, 10, QChar('0'));
//				cv::imwrite(img_filename_Eval.toUtf8().constData(), result_G);
//
//				index++;
//
//			}
//		}
//	}
//	return index;
//}

int GLWidget3D::generateScoreFuseImages(QString facadeImagesPath, int index, int width, int height, float window_displacement, float window_prob, int padding){
	// generate facade images
	double step_W = 0.1;
	double step_H = 0.1;
	int num_W = 0;
	int num_H = 0;
	std::pair<int, int> imageRowsRange(2, 8);
	std::pair<int, int> imageColsRange(2, 8);
	std::pair<int, int> imageGroupsRange(1, 1);
	std::pair<double, double> imageRelativeWidthRange(0.2, 0.8);
	std::pair<double, double> imageRelativeHeightRange(0.2, 0.8);

	if (ceil((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W) - (imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W < 0.01)
		num_W = ceil((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W);
	else
		num_W = floor((imageRelativeWidthRange.second - imageRelativeWidthRange.first) / step_W);

	if (ceil((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H) - (imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H < 0.01)
		num_H = ceil((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H);
	else
		num_H = floor((imageRelativeHeightRange.second - imageRelativeHeightRange.first) / step_H);
	cv::Scalar bg_color(0, 0, 0); // white back ground
	cv::Scalar window_color(255, 255, 255); // black for windows
	int thickness = -1;
	double ratioWidth = 0.0;
	double ratioHeight = 0.0;
	for (int row = imageRowsRange.first; row <= imageRowsRange.second; row++){ // loop row
		for (int col = imageColsRange.first; col <= imageColsRange.second; col++){ // loop col
			int num_iters = 100;
			if (row >= 6 && col >= 6)
				num_iters = 120;
			int num_displacement = 20;
			int num_missing = 20;
			if (int(window_prob) == 1)
				num_missing = 1;
			if (int(1 - window_displacement) == 1)
				num_displacement = 1;

			for (int iter_outers = 0; iter_outers < num_iters * num_displacement * num_missing; ++iter_outers){
				cv::Mat result_A(height, width, CV_8UC1, bg_color);
				cv::Mat result_A_G(height, width, CV_8UC1, bg_color);
				cv::Mat result_B(height, width, CV_8UC1, bg_color);
				cv::Mat result_B_G(height, width, CV_8UC1, bg_color);
				cv::Mat result_G(height, width, CV_8UC1, bg_color);

				if (padding > 0){
					// random padding

					int top = padding;
					int bottom = padding;
					int left = padding;
					int right = padding;
					int borderType = cv::BORDER_CONSTANT;
					cv::copyMakeBorder(result_A, result_A, top, bottom, left, right, borderType, bg_color);
					cv::copyMakeBorder(result_A_G, result_A_G, top, bottom, left, right, borderType, bg_color);
					cv::copyMakeBorder(result_B, result_B, top, bottom, left, right, borderType, bg_color);
					cv::copyMakeBorder(result_B_G, result_B_G, top, bottom, left, right, borderType, bg_color);
					cv::copyMakeBorder(result_G, result_G, top, bottom, left, right, borderType, bg_color);
				}
				/* Borders*/
				int top = 0;
				int bottom = 0;
				int left = 0;
				int right = 0;
				if (row >= 6){
					top = util::genRand(4, 8);
					bottom = util::genRand(4, 8);
				}
				else{
					top = util::genRand(4, 10);
					bottom = util::genRand(4, 10);
				}

				if (col >= 6){
					left = util::genRand(4, 8);
					right = util::genRand(4, 8);
				}
				else{
					left = util::genRand(4, 10);
					right = util::genRand(4, 10);
				}

				/* draw the facade */
				int NR = row;
				int NG = 1;
				int NC = col;
				double threshold = 6.0;
				double FW = (width - left - right) * 1.0 / NC;
				double FH = (height - top - bottom) * 1.0 / NR;
				std::vector<double> WW;
				std::vector<double> WH;
				// generate W and H list
				double AllW = 0.0;
				double AllH = 0.0;
				// compute the min start rw and wh
				int min_start_rw = (threshold / FW - imageRelativeWidthRange.first) / step_W;
				if (min_start_rw < 0)
					min_start_rw = 0;
				int min_start_rh = (threshold / FH - imageRelativeHeightRange.first) / step_H;
				if (min_start_rh < 0)
					min_start_rh = 0;
				// computer the min start rh
				int fixed_relativeW = util::genRand(min_start_rw, num_W + 1);
				float size_ratio = 0.6;
				for (int list = 0; list < col; list++){
					if (util::genRand() > size_ratio){
						int relativeW = util::genRand(min_start_rw, num_W + 1);
						WW.push_back(FW * (relativeW * step_W + imageRelativeWidthRange.first));
						AllW += FW * (relativeW * step_W + imageRelativeWidthRange.first);
					}
					else{
						WW.push_back(FW * (fixed_relativeW * step_W + imageRelativeWidthRange.first));
						AllW += FW * (fixed_relativeW * step_W + imageRelativeWidthRange.first);
					}
				}
				int fixed_relativeH = util::genRand(min_start_rh, num_H + 1);
				for (int list = 0; list < row; list++){
					if (util::genRand() > size_ratio){
						int relativeH = util::genRand(min_start_rh, num_H + 1);
						WH.push_back(FH * (relativeH * step_H + imageRelativeHeightRange.first));
						AllH += FH * (relativeH * step_H + imageRelativeHeightRange.first);
					}
					else{
						WH.push_back(FH * (fixed_relativeH * step_H + imageRelativeHeightRange.first));
						AllH += FH * (fixed_relativeH * step_H + imageRelativeHeightRange.first);
					}
				}

				double width_spacing = 0.0f;
				double height_spacing = 0.0f;
				if (NC > 1)
					width_spacing = (width - AllW - left - right) / (NC - 1);
				if (NR > 1)
					height_spacing = (height - AllH - top - bottom) / (NR - 1);


				// compute spacing adjustment
				// currently only adjust right side
				// e.g. between -0.5 * curW and -0.5 * width_spacing
				std::vector<double> SpacingAdjustW;
				std::vector<double> SpacingAdjustH;
				float adjust_ratio = 0.6;
				for (int list = 0; list < col; list++){
					if (list < col - 1){
						if (util::genRand() > adjust_ratio) // 50% addding spacing adjustment
							SpacingAdjustW.push_back(util::genRand(-0.5 * WW[list], 0.5 * width_spacing));
						else
							SpacingAdjustW.push_back(0);
					}
					else
						SpacingAdjustW.push_back(0);
				}
				for (int list = 0; list < row; list++){
					if (list < row - 1){
						if (util::genRand() > adjust_ratio)
							SpacingAdjustH.push_back(util::genRand(-0.5 * WH[list], 0.5 * height_spacing));
						else
							SpacingAdjustH.push_back(0);
					}
					else
						SpacingAdjustH.push_back(0);
				}

				// check valid
				bool bValid_Window = true;
				for (int list = 0; list < WW.size(); list++){
					if (WW[list] + SpacingAdjustW[list] < threshold){
						bValid_Window = false;
						break;
					}
				}
				for (int list = 0; list < WH.size(); list++){
					if (WH[list] + SpacingAdjustH[list] < threshold){
						bValid_Window = false;
						break;
					}
				}

				// check valid of spacing
				bool bValid_Spacing = true;
				for (int list = 0; list < WW.size(); list++){
					if (width_spacing - SpacingAdjustW[list] < threshold){
						bValid_Spacing = false;
						break;
					}
				}
				for (int list = 0; list < WH.size(); list++){
					if (height_spacing - SpacingAdjustH[list] < threshold){
						bValid_Spacing = false;
						break;
					}
				}

				if (!bValid_Window || !bValid_Spacing){
					//std::cout << "Invalid image" << std::endl;
					continue;
				}

				if (NG == 1){
					float curH_spacing = top;
					float curW_spacing = left;
					for (int i = 0; i < NR; ++i) {
						curW_spacing = left;
						for (int j = 0; j < NC; ++j) {
							float x1_A, y1_A, x2_A, y2_A;
							float x1_A_G, y1_A_G, x2_A_G, y2_A_G;
							float x1_B, y1_B, x2_B, y2_B;
							float x1_B_G, y1_B_G, x2_B_G, y2_B_G;
							float x1_G, y1_G, x2_G, y2_G;

							float curW = WW[j];
							float curH = WH[i];

							float spacing_adjustW = SpacingAdjustW[j];
							float spacing_adjustH = SpacingAdjustH[i];


							// A
							x1_A = curW_spacing;
							y1_A = curH_spacing;
							x2_A = x1_A + curW + spacing_adjustW;
							y2_A = y1_A + curH + spacing_adjustH;
							// A_G
							x1_A_G = curW_spacing;
							y1_A_G = curH_spacing;
							x2_A_G = x1_A_G + curW + spacing_adjustW;
							y2_A_G = y1_A_G + curH + spacing_adjustH;
							// B
							x1_B = curW_spacing;
							y1_B = curH_spacing;
							x2_B = x1_B + curW + spacing_adjustW;
							y2_B = y1_B + curH + spacing_adjustH;
							// B_G
							x1_B_G = curW_spacing;
							y1_B_G = curH_spacing;
							x2_B_G = x1_B_G + curW + spacing_adjustW;
							y2_B_G = y1_B_G + curH + spacing_adjustH;
							//G
							x1_G = curW_spacing;
							y1_G = curH_spacing;
							x2_G = x1_G + curW + spacing_adjustW;
							y2_G = y1_G + curH + spacing_adjustH;

							if (window_displacement > 0) {
								double w_displacment = curW * window_displacement < 0.5 * threshold ? curW * window_displacement : 0.5 * threshold;
								double h_displacment = curH * window_displacement < 0.5 * threshold ? curH * window_displacement : 0.5 * threshold;
								x1_A += util::genRand(-w_displacment, w_displacment);
								y1_A += util::genRand(-h_displacment, h_displacment);
								x2_A += util::genRand(-w_displacment, w_displacment);
								y2_A += util::genRand(-h_displacment, h_displacment);

								// can not be too small
								if (x2_A - x1_A < threshold)
								{
									x1_A = curW_spacing;
									x2_A = x1_A + curW + spacing_adjustW;
								}
								if (y2_A - y1_A < threshold){
									y1_A = curH_spacing;
									y2_A = y1_A + curH + spacing_adjustH;
								}

								x1_B += util::genRand(-w_displacment, w_displacment);
								y1_B += util::genRand(-h_displacment, h_displacment);
								x2_B += util::genRand(-w_displacment, w_displacment);
								y2_B += util::genRand(-h_displacment, h_displacment);

								// can not be too small
								if (x2_B - x1_B < threshold)
								{
									x1_B = curW_spacing;
									x2_B = x1_B + curW + spacing_adjustW;
								}
								if (y2_B - y1_B < threshold){
									y1_B = curH_spacing;
									y2_B = y1_B + curH + spacing_adjustH;
								}

							}

							if (util::genRand() < window_prob) {
								if (padding > 0){
									cv::rectangle(result_A, cv::Point(std::round(x1_A) + padding, std::round(y1_A) + padding), cv::Point(std::round(x2_A) + padding, std::round(y2_A) + padding), window_color, thickness);
									cv::rectangle(result_A_G, cv::Point(std::round(x1_A_G) + padding, std::round(y1_A_G) + padding), cv::Point(std::round(x2_A_G) + padding, std::round(y2_A_G) + padding), window_color, thickness);
								}
								else{
									cv::rectangle(result_A, cv::Point(std::round(x1_A), std::round(y1_A)), cv::Point(std::round(x2_A), std::round(y2_A)), window_color, thickness);
									cv::rectangle(result_A_G, cv::Point(std::round(x1_A_G), std::round(y1_A_G)), cv::Point(std::round(x2_A_G), std::round(y2_A_G)), window_color, thickness);
								}
							}

							if (util::genRand() < window_prob) {
								if (padding > 0){
									cv::rectangle(result_B, cv::Point(std::round(x1_B) + padding, std::round(y1_B) + padding), cv::Point(std::round(x2_B) + padding, std::round(y2_B) + padding), window_color, thickness);
									cv::rectangle(result_B_G, cv::Point(std::round(x1_B_G) + padding, std::round(y1_B_G) + padding), cv::Point(std::round(x2_B_G) + padding, std::round(y2_B_G) + padding), window_color, thickness);
								}
								else{
									cv::rectangle(result_B, cv::Point(std::round(x1_B), std::round(y1_B)), cv::Point(std::round(x2_B), std::round(y2_B)), window_color, thickness);
									cv::rectangle(result_B_G, cv::Point(std::round(x1_B_G) , std::round(y1_B_G)), cv::Point(std::round(x2_B_G), std::round(y2_B_G)), window_color, thickness);
								}
							}
							if (padding > 0){
								cv::rectangle(result_G, cv::Point(std::round(x1_G) + padding, std::round(y1_G) + padding), cv::Point(std::round(x2_G) + padding, std::round(y2_G) + padding), window_color, thickness);
							}
							else
								cv::rectangle(result_G, cv::Point(std::round(x1_G), std::round(y1_G)), cv::Point(std::round(x2_G), std::round(y2_G)), window_color, thickness);

							curW_spacing += curW + width_spacing;
						}
						curH_spacing += height_spacing + WH[i];
					}
				}
				// A
				QString img_filename_A = facadeImagesPath + QString("/A/facade_%1.png").arg(index, 5, 10, QChar('0'));
				cv::imwrite(img_filename_A.toUtf8().constData(), result_A);
				// A_G
				QString img_filename_A_G = facadeImagesPath + QString("/A_G/facade_%1.png").arg(index, 5, 10, QChar('0'));
				cv::imwrite(img_filename_A_G.toUtf8().constData(), result_A_G);
				//{
				//	// B
				//	QString img_filename_B = facadeImagesPath + QString("/B/facade_%1.png").arg(index, 5, 10, QChar('0'));
				//	cv::imwrite(img_filename_B.toUtf8().constData(), result_B);
				//	// B_G
				//	QString img_filename_B_G = facadeImagesPath + QString("/B_G/facade_%1.png").arg(index, 5, 10, QChar('0'));
				//	cv::imwrite(img_filename_B_G.toUtf8().constData(), result_B_G);

				//	// fuse image
				//	cv::Mat fuse_img = fuse_images(result_A_G, result_B_G);
				//	QString img_filename_G = facadeImagesPath + QString("/G/facade_%1.png").arg(index, 5, 10, QChar('0'));
				//	cv::imwrite(img_filename_G.toUtf8().constData(), fuse_img);

				//	// Eval image
				//	QString img_filename_Eval = facadeImagesPath + QString("/Eval/facade_%1.png").arg(index, 5, 10, QChar('0'));
				//	cv::imwrite(img_filename_Eval.toUtf8().constData(), result_G);
				//}

				//std::cout << img_filename_A.toUtf8().constData() << std::endl;
				index++;

			}
		}
	}
	return index;
}


std::vector<double> GLWidget3D::eval_accuracy(const cv::Mat& seg_img, const cv::Mat& gt_img) {
	int gt_p = 0;
	int seg_tp = 0;
	int seg_fn = 0;
	int gt_n = 0;
	int seg_tn = 0;
	int seg_fp = 0;
	for (int i = 0; i < gt_img.size().height; i++) {
		for (int j = 0; j < gt_img.size().width; j++) {
			// window
			if ((int)gt_img.at<uchar>(i, j) == 255) {
				gt_p++;
				if ((int)seg_img.at<uchar>(i, j) == 255) {
					seg_tp++;
				}
				else
					seg_fn++;
			}
			else {// wall
				gt_n++;
				if ((int)seg_img.at<uchar>(i, j) == 0) {
					seg_tn++;
				}
				else
					seg_fp++;
			}
		}
	}
	// return pixel accuracy and class accuracy
	std::vector<double> eval_metrix;
	// accuracy 
	eval_metrix.push_back(1.0 * (seg_tp + seg_tn) / (gt_p + gt_n));
	// precision
	double precision = 1.0 * seg_tp / (seg_tp + seg_fp);
	// recall
	double recall = 1.0 * seg_tp / (seg_tp + seg_fn);
	eval_metrix.push_back(precision);
	eval_metrix.push_back(recall);
	double f_score = 2 * precision * recall / (precision + recall);
	eval_metrix.push_back(f_score);
	std::cout << "P = " << gt_p << std::endl;
	std::cout << "N = " << gt_n << std::endl;
	std::cout << "TP = " << seg_tp << std::endl;
	std::cout << "FN = " << seg_fn << std::endl;
	std::cout << "TN = " << seg_tn << std::endl;
	std::cout << "FP = " << seg_fp << std::endl;
	return eval_metrix;
}

cv::Mat GLWidget3D::fuse_images(const cv::Mat& src_img_1, const cv::Mat& src_img_2) {
	cv::Scalar bg_color(0, 0, 0); // white back ground
	cv::Scalar window_color(255, 255, 255); // black for windows
	cv::Mat result(src_img_1.size(), CV_8UC1, bg_color);
	for (int i = 0; i < src_img_1.size().height; i++) {
		for (int j = 0; j < src_img_1.size().width; j++) {
			// window
			if ((int)src_img_1.at<uchar>(i, j) == 255 || (int)src_img_2.at<uchar>(i, j) == 255) {
				result.at<uchar>(i, j) = (uchar)255;
			}
			else {// wall
				result.at<uchar>(i, j) = (uchar)0;
			}
		}
	}
	return result;
}

void GLWidget3D::update3DGeometry() {
	if (show_mode == SHOW_INPUT) {
		update3DGeometry(voxel_buildings);
	}
	else {
		update3DGeometryWithoutRoof(buildings);
	}
}

/**
* Update 3D geometry using the input building shapes.
*
* @param buildings		buildings
*/
void GLWidget3D::update3DGeometry(const std::vector<util::VoxelBuilding>& voxel_buildings) {
	renderManager.removeObjects();

	glm::vec4 color(0.7, 1, 0.7, 1);

	std::vector<Vertex> vertices;
	for (int i = 0; i < voxel_buildings.size(); i++) {
		for (int z = 0; z < voxel_buildings[i].node_stack.size(); z++) {
			for (auto voxel_node : voxel_buildings[i].node_stack[z]) {
				update3DGeometry(voxel_node, color, vertices);
			}
		}
	}
	renderManager.addObject("building", "", vertices, true);

	std::vector<Vertex> vertices2;
	glutils::drawBox(3000, 3000, 0.5, glm::vec4(0.9, 1, 0.9, 1), glm::translate(glm::mat4(), glm::vec3(0, 0, -0.5)), vertices2);
	renderManager.addObject("ground", "", vertices2, true);

	// update shadow map
	renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);
}

void GLWidget3D::update3DGeometry(const std::shared_ptr<util::VoxelNode>& voxel_node, glm::vec4& color, std::vector<Vertex>& vertices) {
	std::vector<glm::dvec2> footprint(voxel_node->contour.contour.size());
	for (int i = 0; i < voxel_node->contour.contour.size(); i++) {
		cv::Point2f pt = voxel_node->contour.contour.getActualPoint(i);
		footprint[i] = glm::dvec2(pt.x * scale, pt.y * scale);
	}
	std::vector<std::vector<glm::dvec2>> holes(voxel_node->contour.holes.size());
	for (int i = 0; i < voxel_node->contour.holes.size(); i++) {
		if (voxel_node->contour.holes[i].size() < 3) continue;
		holes[i].resize(voxel_node->contour.holes[i].size());
		for (int j = 0; j < voxel_node->contour.holes[i].size(); j++) {
			cv::Point2f pt = voxel_node->contour.holes[i].getActualPoint(j);
			holes[i][j] = glm::dvec2(pt.x * scale, pt.y * scale);
		}
	}

	// correct the orientation of polygon
	glutils::correct(footprint);
	for (int i = 0; i < holes.size(); i++) {
		glutils::correct(holes[i]);
	}

	glm::mat4 mat = glm::translate(glm::mat4(), glm::vec3(0, 0, voxel_node->height * scale));
	double h = scale;

	// top face
	glutils::drawConcavePolygon(footprint, holes, color, glm::translate(mat, glm::vec3(0, 0, h)), vertices);

	// bottom face
	glutils::drawConcavePolygon(footprint, holes, color, mat, vertices, true);

	// side faces
	for (int i = 0; i < footprint.size(); i++) {
		int next = (i + 1) % footprint.size();

		glm::vec3 p1(mat * glm::vec4(footprint[i], 0, 1));
		glm::vec3 p2(mat * glm::vec4(footprint[next], 0, 1));
		glm::vec3 p3(mat * glm::vec4(footprint[next], h, 1));
		glm::vec3 p4(mat * glm::vec4(footprint[i], h, 1));

		glm::vec3 n = glm::cross(p2 - p1, p3 - p2);
		n /= glm::length(n);

		vertices.push_back(Vertex(p1, n, color));
		vertices.push_back(Vertex(p2, n, color));
		vertices.push_back(Vertex(p3, n, color));

		vertices.push_back(Vertex(p1, n, color));
		vertices.push_back(Vertex(p3, n, color));
		vertices.push_back(Vertex(p4, n, color));
	}

	// side faces for the holes
	for (auto& hole : holes) {
		std::reverse(hole.begin(), hole.end());
		for (int j = 0; j < hole.size(); j++) {
			int next = (j + 1) % hole.size();

			glm::vec3 p1(mat * glm::vec4(hole[j], 0, 1));
			glm::vec3 p2(mat * glm::vec4(hole[next], 0, 1));
			glm::vec3 p3(mat * glm::vec4(hole[next], h, 1));
			glm::vec3 p4(mat * glm::vec4(hole[j], h, 1));

			glm::vec3 n = glm::cross(p2 - p1, p3 - p2);
			n /= glm::length(n);

			vertices.push_back(Vertex(p1, n, color));
			vertices.push_back(Vertex(p2, n, color));
			vertices.push_back(Vertex(p3, n, color));

			vertices.push_back(Vertex(p1, n, color));
			vertices.push_back(Vertex(p3, n, color));
			vertices.push_back(Vertex(p4, n, color));
		}
	}
}

/**
 * Update 3D geometry using the simplified building shapes.
 * Use the generated primitive shapes to create the 3D mesh.
 *
 * @param buildings		buildings
 */
void GLWidget3D::update3DGeometryWithoutRoof(const std::vector<std::shared_ptr<util::BuildingLayer>>& buildings) {
	renderManager.removeObjects();

	glm::vec4 color(0.7, 1, 0.7, 1);

	QMap<QString, std::vector<Vertex>> vertices;
	for (int i = 0; i < buildings.size(); i++) {
		// randomly select a texture file
		int facade_texture_id = rand() % 13;
		if (color_mode == TEXTURE) {
			if (facade_texture_id == 0) {
				color = glm::vec4(0.741, 0.623, 0.490, 1.0);
			}
			else if (facade_texture_id == 1) {
				color = glm::vec4(0.792, 0.768, 0.721, 1.0);
			}
			else if (facade_texture_id == 2) {
				color = glm::vec4(0.772, 0.709, 0.647, 1.0);
			}
			else if (facade_texture_id == 3) {
				color = glm::vec4(0.674, 0.639, 0.572, 1.0);
			}
			else if (facade_texture_id == 4) {
				color = glm::vec4(0.776, 0.819, 0.843, 1.0);
			}
			else if (facade_texture_id == 5) {
				color = glm::vec4(0.882, 0.882, 0.874, 1.0);
			}
			else if (facade_texture_id == 6) {
				color = glm::vec4(0.811, 0.811, 0.803, 1.0);
			}
			else if (facade_texture_id == 7) {
				color = glm::vec4(0.850, 0.831, 0.815, 1.0);
			}
			else if (facade_texture_id == 8) {
				color = glm::vec4(0.811, 0.831, 0.843, 1.0);
			}
			else if (facade_texture_id == 9) {
				color = glm::vec4(0.784, 0.721, 0.623, 1.0);
			}
			else if (facade_texture_id == 10) {
				color = glm::vec4(0.494, 0.388, 0.321, 1.0);
			}
			else if (facade_texture_id == 11) {
				color = glm::vec4(0.756, 0.780, 0.772, 1.0);
			}
			else if (facade_texture_id == 12) {
				color = glm::vec4(0.764, 0.768, 0.788, 1.0);
			}
		}

		QString facade_texture = QString("textures/window_tile%1.png").arg(facade_texture_id);
		QString roof_texture = QString("textures/roof%1.png").arg(rand() % 4);
		update3DGeometryWithoutRoof(buildings[i], color, facade_texture, roof_texture, vertices);
	}
	for (auto it = vertices.begin(); it != vertices.end(); it++) {
		renderManager.addObject("building", it.key(), it.value(), true);
	}

	std::vector<Vertex> vertices2;
	glutils::drawBox(3000, 3000, 0.5, glm::vec4(0.9, 1, 0.9, 1), glm::translate(glm::mat4(), glm::vec3(0, 0, -0.5)), vertices2);
	renderManager.addObject("ground", "", vertices2, true);

	// update shadow map
	renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);
}

void GLWidget3D::update3DGeometryWithoutRoof(std::shared_ptr<util::BuildingLayer> building, glm::vec4& color, const QString& facade_texture, const QString& roof_texture, QMap<QString, std::vector<Vertex>>& vertices) {
	float floor_tile_width = 2.4f;
	float floor_tile_height = 3.0f;

	for (auto& bf : building->footprints) {
		if (bf.contour.size() == 0) continue;

		std::vector<std::vector<cv::Point2f>> polygons;

		polygons = util::tessellate(bf.contour, bf.holes);
		for (int i = 0; i < polygons.size(); i++) {
			util::transform(polygons[i], bf.mat);
		}

		glm::mat4 mat = glm::translate(glm::mat4(), glm::vec3(0, 0, building->bottom_height * scale));
		double height = (building->top_height - building->bottom_height) * scale;

		for (auto polygon : polygons) {
			util::clockwise(polygon);

			std::vector<glm::dvec2> pol(polygon.size());

			for (int i = 0; i < polygon.size(); i++) {
				pol[i] = glm::dvec2(polygon[i].x * scale, polygon[i].y * scale);
			}

			// top face
			glutils::drawPolygon(pol, color, glm::translate(glm::mat4(), glm::vec3(0, 0, building->top_height * scale)), vertices[""]);

			// bottom face
			glutils::drawPolygon(pol, color, mat, vertices[""], true);
		}

		util::Ring ring = bf.contour.getActualPoints();
		ring.counterClockwise();
		std::vector<glm::dvec2> pol(ring.size());
		for (int i = 0; i < ring.size(); i++) {
			pol[i] = glm::dvec2(ring[i].x * scale, ring[i].y * scale);
		}

		// side faces

		// At first, find the first point that has angle close to 90 degrees.
		int start_index = 0;
		for (int i = 0; i < pol.size(); i++) {
			int prev = (i + pol.size() - 1) % pol.size();
			int next = (i + 1) % pol.size();

			if (dotProductBetweenThreePoints(pol[prev], pol[i], pol[next]) < 0.8) {
				start_index = i;
				break;
			}
		}

		std::vector<glm::dvec2> coords;
		for (int i = 0; i <= pol.size(); i++) {
			int prev = (start_index + i - 1 + pol.size()) % pol.size();
			int cur = (start_index + i) % pol.size();
			int next = (start_index + i + 1) % pol.size();

			if (dotProductBetweenThreePoints(pol[prev], pol[cur], pol[next]) < 0.8) {
				// create a face
				if (coords.size() > 0) {
					coords.push_back(pol[cur]);
					createFace(coords, height, floor_tile_width, floor_tile_height, mat, color, facade_texture, vertices);
				}
				coords.clear();
			}

			coords.push_back(pol[cur]);
		}

		// create a face
		if (coords.size() >= 2) {
			createFace(coords, height, floor_tile_width, floor_tile_height, mat, color, facade_texture, vertices);
		}

		// side faces of holes
		for (auto& bh : bf.holes) {
			util::Ring ring = bh.getActualPoints();
			ring.clockwise();
			std::vector<glm::dvec2> pol(ring.size());
			for (int i = 0; i < ring.size(); i++) {
				pol[i] = glm::dvec2(ring[i].x * scale, ring[i].y * scale);
			}

			// side faces

			// At first, find the first point that has angle close to 90 degrees.
			int start_index = 0;
			for (int i = 0; i < pol.size(); i++) {
				int prev = (i + pol.size() - 1) % pol.size();
				int next = (i + 1) % pol.size();

				if (dotProductBetweenThreePoints(pol[prev], pol[i], pol[next]) < 0.8) {
					start_index = i;
					break;
				}
			}

			std::vector<glm::dvec2> coords;
			for (int i = 0; i <= pol.size(); i++) {
				int prev = (start_index + i - 1 + pol.size()) % pol.size();
				int cur = (start_index + i) % pol.size();
				int next = (start_index + i + 1) % pol.size();

				if (dotProductBetweenThreePoints(pol[prev], pol[cur], pol[next]) < 0.8) {
					// create a face
					if (coords.size() > 0) {
						coords.push_back(pol[cur]);
						createFace(coords, height, floor_tile_width, floor_tile_height, mat, color, facade_texture, vertices);
					}
					coords.clear();
				}

				coords.push_back(pol[cur]);
			}

			// create a face
			if (coords.size() >= 2) {
				createFace(coords, height, floor_tile_width, floor_tile_height, mat, color, facade_texture, vertices);
			}
		}
	}

	for (auto child : building->children) {
		update3DGeometryWithoutRoof(child, color, facade_texture, roof_texture, vertices);
	}
}

/**
 * Update 3D geometry using the simplified building shapes.
 * Use the contour polygon to generate a flat roof.
 *
 * @param buildings		buildings
 */
/*
void GLWidget3D::update3DGeometryWithRoof(const std::vector<std::shared_ptr<util::BuildingLayer>>& buildings) {
	renderManager.removeObjects();

	glm::vec4 color(0.7, 1, 0.7, 1);

	QMap<QString, std::vector<Vertex>> vertices;
	for (int i = 0; i < buildings.size(); i++) {
		// randomly select a texture file
		int facade_texture_id = rand() % 13;
		if (color_mode == TEXTURE) {
			if (facade_texture_id == 0) {
				color = glm::vec4(0.741, 0.623, 0.490, 1.0);
			}
			else if (facade_texture_id == 1) {
				color = glm::vec4(0.792, 0.768, 0.721, 1.0);
			}
			else if (facade_texture_id == 2) {
				color = glm::vec4(0.772, 0.709, 0.647, 1.0);
			}
			else if (facade_texture_id == 3) {
				color = glm::vec4(0.674, 0.639, 0.572, 1.0);
			}
			else if (facade_texture_id == 4) {
				color = glm::vec4(0.776, 0.819, 0.843, 1.0);
			}
			else if (facade_texture_id == 5) {
				color = glm::vec4(0.882, 0.882, 0.874, 1.0);
			}
			else if (facade_texture_id == 6) {
				color = glm::vec4(0.811, 0.811, 0.803, 1.0);
			}
			else if (facade_texture_id == 7) {
				color = glm::vec4(0.850, 0.831, 0.815, 1.0);
			}
			else if (facade_texture_id == 8) {
				color = glm::vec4(0.811, 0.831, 0.843, 1.0);
			}
			else if (facade_texture_id == 9) {
				color = glm::vec4(0.784, 0.721, 0.623, 1.0);
			}
			else if (facade_texture_id == 10) {
				color = glm::vec4(0.494, 0.388, 0.321, 1.0);
			}
			else if (facade_texture_id == 11) {
				color = glm::vec4(0.756, 0.780, 0.772, 1.0);
			}
			else if (facade_texture_id == 12) {
				color = glm::vec4(0.764, 0.768, 0.788, 1.0);
			}
		}

		QString facade_texture = QString("textures/window_tile%1.png").arg(facade_texture_id);
		QString roof_texture = QString("textures/roof%1.png").arg(rand() % 4);
		update3DGeometryWithRoof(buildings[i], color, facade_texture, roof_texture, vertices);
	}
	for (auto it = vertices.begin(); it != vertices.end(); it++) {
		renderManager.addObject("building", it.key(), it.value(), true);
	}

	std::vector<Vertex> vertices2;
	glutils::drawBox(1500, 1500, 0.5, glm::vec4(0.9, 1, 0.9, 1), glm::translate(glm::mat4(), glm::vec3(0, 0, -0.5)), vertices2);
	renderManager.addObject("ground", "", vertices2, true);

	// update shadow map
	renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);
}
*/

/*
void GLWidget3D::update3DGeometryWithRoof(std::shared_ptr<util::BuildingLayer> building, glm::vec4& color, const QString& facade_texture, const QString& roof_texture, QMap<QString, std::vector<Vertex>>& vertices) {
	if (building->footprint.contour.size() < 3) return;

	std::vector<glm::dvec2> footprint(building->footprint.contour.size());
	for (int j = 0; j < building->footprint.contour.size(); j++) {
		cv::Point2f pt = building->footprint.contour.getActualPoint(j);
		footprint[j] = glm::dvec2(pt.x, pt.y);
	}
	std::vector<std::vector<glm::dvec2>> holes(building->footprint.holes.size());
	for (int j = 0; j < building->footprint.holes.size(); j++) {
		if (building->footprint.holes[j].size() < 3) continue;
		holes[j].resize(building->footprint.holes[j].size());
		for (int k = 0; k < building->footprint.holes[j].size(); k++) {
			cv::Point2f pt = building->footprint.holes[j].getActualPoint(k);
			holes[j][k] = glm::dvec2(pt.x, pt.y);
		}
	}

	// correct the orientation of polygon
	glutils::correct(footprint);
	for (int i = 0; i < holes.size(); i++) {
		glutils::correct(holes[i]);
	}

	glm::mat4 mat = glm::translate(glm::mat4(), glm::vec3(0, 0, building->bottom_height));
	double h = building->top_height - building->bottom_height;

	// top face
	if (color_mode == COLOR) {
		glutils::drawConcavePolygon(footprint, holes, color, glm::translate(mat, glm::vec3(0, 0, h)), vertices[""]);
	}
	else {
		glm::mat4 topface_mat = glm::translate(mat, glm::vec3(0, 0, h));

		// dilate the contour
		std::vector<glm::dvec2> dilated_footprint;
		glutils::offsetPolygon(footprint, 0.2, dilated_footprint);
		std::vector<std::vector<glm::dvec2>> dilated_holes(holes.size());
		for (int i = 0; i < dilated_holes.size(); i++) {
			glutils::offsetPolygon(holes[i], -0.2, dilated_holes[i]);
		}

		// bottom face of the roof
		glutils::drawConcavePolygon(dilated_footprint, dilated_holes, color, topface_mat, vertices[""], true);

		// side faces of the roof
		double roof_ledge_height = 0.2;
		for (int i = 0; i < dilated_footprint.size(); i++) {
			int next = (i + 1) % dilated_footprint.size();
			glm::vec3 p1(topface_mat * glm::vec4(dilated_footprint[i], 0, 1));
			glm::vec3 p2(topface_mat * glm::vec4(dilated_footprint[next], 0, 1));
			glm::vec3 p3(topface_mat * glm::vec4(dilated_footprint[next], roof_ledge_height, 1));
			glm::vec3 p4(topface_mat * glm::vec4(dilated_footprint[i], roof_ledge_height, 1));

			glm::vec3 n = glm::cross(p2 - p1, p3 - p2);
			n /= glm::length(n);

			vertices[""].push_back(Vertex(p1, n, color));
			vertices[""].push_back(Vertex(p2, n, color));
			vertices[""].push_back(Vertex(p3, n, color));

			vertices[""].push_back(Vertex(p1, n, color));
			vertices[""].push_back(Vertex(p3, n, color));
			vertices[""].push_back(Vertex(p4, n, color));
		}

		// side faces for the holes of the roof
		for (int i = 0; i < dilated_holes.size(); i++) {
			reverse(dilated_holes[i].begin(), dilated_holes[i].end());
			for (int j = 0; j < dilated_holes[i].size(); j++) {
				int next = (j + 1) % dilated_holes[i].size();
				glm::vec3 p1(topface_mat * glm::vec4(dilated_holes[i][j], 0, 1));
				glm::vec3 p2(topface_mat * glm::vec4(dilated_holes[i][next], 0, 1));
				glm::vec3 p3(topface_mat * glm::vec4(dilated_holes[i][next], roof_ledge_height, 1));
				glm::vec3 p4(topface_mat * glm::vec4(dilated_holes[i][j], roof_ledge_height, 1));

				glm::vec3 n = glm::cross(p2 - p1, p3 - p2);
				n /= glm::length(n);

				// the facade is too small, so we use a simple color
				vertices[""].push_back(Vertex(p1, n, color));
				vertices[""].push_back(Vertex(p2, n, color));
				vertices[""].push_back(Vertex(p3, n, color));

				vertices[""].push_back(Vertex(p1, n, color));
				vertices[""].push_back(Vertex(p3, n, color));
				vertices[""].push_back(Vertex(p4, n, color));
			}
		}

		// top of the roof
		std::vector<glm::dvec2> texCoords(dilated_footprint.size());
		std::vector<std::vector<glm::dvec2>> holes_texCoords(dilated_holes.size());
		glutils::BoundingBox bbox(dilated_footprint);
		for (int i = 0; i < dilated_footprint.size(); i++) {
			texCoords[i] = glm::vec2((dilated_footprint[i].x - bbox.minPt.x) / 10, (dilated_footprint[i].y - bbox.minPt.y) / 10);
		}
		for (int i = 0; i < dilated_holes.size(); i++) {
			holes_texCoords[i].resize(holes[i].size());
			for (int j = 0; j < holes[i].size(); j++) {
				holes_texCoords[i][j] = glm::vec2((dilated_holes[i][j].x - bbox.minPt.x) / 10, (dilated_holes[i][j].y - bbox.minPt.y) / 10);
			}
		}
		glutils::drawConcavePolygon(dilated_footprint, dilated_holes, glm::vec4(1, 1, 1, 1), texCoords, holes_texCoords, glm::translate(topface_mat, glm::vec3(0, 0, roof_ledge_height)), vertices[roof_texture]);
	}

	// bottom face
	glutils::drawConcavePolygon(footprint, holes, color, mat, vertices[""], true);


	// side faces
	float floor_tile_width = 3.0f;
	float floor_tile_height = 3.0f;

	// At first, find the first point that has angle close to 90 degrees.
	int start_index = 0;
	for (int i = 0; i < footprint.size(); i++) {
		int prev = (i + footprint.size() - 1) % footprint.size();
		int next = (i + 1) % footprint.size();

		if (dotProductBetweenThreePoints(footprint[prev], footprint[i], footprint[next]) < 0.8) {
			start_index = i;
			break;
		}
	}

	std::vector<glm::dvec2> coords;
	for (int i = 0; i <= footprint.size(); i++) {
		int prev = (start_index + i - 1 + footprint.size()) % footprint.size();
		int cur = (start_index + i) % footprint.size();
		int next = (start_index + i + 1) % footprint.size();

		if (dotProductBetweenThreePoints(footprint[prev], footprint[cur], footprint[next]) < 0.8) {
			// create a face
			if (coords.size() > 0) {
				coords.push_back(footprint[cur]);
				createFace(coords, h, floor_tile_width, floor_tile_height, mat, color, facade_texture, vertices);
			}
			coords.clear();
		}

		coords.push_back(footprint[cur]);
	}

	// create a face
	if (coords.size() >= 2) {
		createFace(coords, h, floor_tile_width, floor_tile_height, mat, color, facade_texture, vertices);
	}

	// side faces for the holes
	for (int i = 0; i < holes.size(); i++) {
		reverse(holes[i].begin(), holes[i].end());

		// At first, find the first point that has angle close to 90 degrees.
		int start_index = 0;
		for (int j = 0; j < holes[i].size(); j++) {
			int prev = (j + holes[i].size() - 1) % holes[i].size();
			int next = (j + 1) % holes[i].size();

			if (dotProductBetweenThreePoints(holes[i][prev], holes[i][i], holes[i][next]) < 0.8) {
				start_index = j;
				break;
			}
		}

		std::vector<glm::dvec2> coords;
		for (int j = 0; j <= holes[i].size(); j++) {
			int prev = (start_index + j - 1 + holes[i].size()) % holes[i].size();
			int cur = (start_index + j) % holes[i].size();
			int next = (start_index + j + 1) % holes[i].size();

			if (dotProductBetweenThreePoints(holes[i][prev], holes[i][cur], holes[i][next]) < 0.8) {
				// create a face
				if (coords.size() > 0) {
					coords.push_back(holes[i][cur]);
					createFace(coords, h, floor_tile_width, floor_tile_height, mat, color, facade_texture, vertices);
				}
				coords.clear();
			}

			coords.push_back(holes[i][cur]);
		}

		// create a face
		if (coords.size() >= 2) {
			createFace(coords, h, floor_tile_width, floor_tile_height, mat, color, facade_texture, vertices);
		}
	}

	if (building->child) {
		update3DGeometryWithRoof(building->child, color, facade_texture, roof_texture, vertices);
	}
}
*/

void GLWidget3D::createFace(const std::vector<glm::dvec2>& coords, double h, float floor_tile_width, float floor_tile_height, const glm::mat4& mat, glm::vec4& color, const QString& facade_texture, QMap<QString, std::vector<Vertex>>& vertices) {
	float length = getLength(coords);
	int repetition = length / floor_tile_width;
	float actual_floor_tile_width = 0;
	if (repetition > 0) actual_floor_tile_width = length / repetition;

	float tex_x = 0;
	for (int i = 1; i < coords.size(); i++) {
		glm::vec3 p1(mat * glm::vec4(coords[i - 1], 0, 1));
		glm::vec3 p2(mat * glm::vec4(coords[i], 0, 1));
		glm::vec3 p3(mat * glm::vec4(coords[i], h, 1));
		glm::vec3 p4(mat * glm::vec4(coords[i - 1], h, 1));

		glm::vec2 t1(tex_x, 0);
		float tex_dx = 0;
		if (repetition > 0) {
			tex_dx = glm::length(coords[i] - coords[i - 1]) / actual_floor_tile_width;
		}
		int tex_coord_y = h / floor_tile_height;
		glm::vec2 t2(tex_x + tex_dx, 0);
		glm::vec2 t3(tex_x + tex_dx, tex_coord_y);
		glm::vec2 t4(tex_x, tex_coord_y);

		tex_x += tex_dx;

		glm::vec3 n = glm::cross(p2 - p1, p3 - p2);
		n /= glm::length(n);

		if (color_mode == COLOR || repetition == 0 || tex_coord_y == 0) {
			// the facade is too small, so we use a simple color
			vertices[""].push_back(Vertex(p1, n, color));
			vertices[""].push_back(Vertex(p2, n, color));
			vertices[""].push_back(Vertex(p3, n, color));

			vertices[""].push_back(Vertex(p1, n, color));
			vertices[""].push_back(Vertex(p3, n, color));
			vertices[""].push_back(Vertex(p4, n, color));
		}
		else {
			// the facade is big enough for texture mapping
			vertices[facade_texture].push_back(Vertex(p1, n, glm::vec4(1, 1, 1, 1), t1));
			vertices[facade_texture].push_back(Vertex(p2, n, glm::vec4(1, 1, 1, 1), t2));
			vertices[facade_texture].push_back(Vertex(p3, n, glm::vec4(1, 1, 1, 1), t3));

			vertices[facade_texture].push_back(Vertex(p1, n, glm::vec4(1, 1, 1, 1), t1));
			vertices[facade_texture].push_back(Vertex(p3, n, glm::vec4(1, 1, 1, 1), t3));
			vertices[facade_texture].push_back(Vertex(p4, n, glm::vec4(1, 1, 1, 1), t4));
		}		
	}
}

double GLWidget3D::dotProductBetweenThreePoints(const glm::dvec2& a, const glm::dvec2& b, const glm::dvec2& c) {
	glm::dvec2 prev_vec = b - a;
	glm::dvec2 next_vec = c - b;
	prev_vec /= glm::length(prev_vec);
	next_vec /= glm::length(next_vec);
	return glm::dot(prev_vec, next_vec);
}

double GLWidget3D::getLength(const std::vector<glm::dvec2>& points) {
	double ans = 0;
	for (int i = 1; i < points.size(); i++) {
		ans += glm::length(points[i] - points[i - 1]);
	}
	return ans;
}

void GLWidget3D::keyPressEvent(QKeyEvent *e) {
	ctrlPressed = false;
	shiftPressed = false;

	if (e->modifiers() & Qt::ControlModifier) {
		ctrlPressed = true;
	}
	if (e->modifiers() & Qt::ShiftModifier) {
		shiftPressed = true;
	}

	switch (e->key()) {
	case Qt::Key_Space:
		break;
	default:
		break;
	}
}

void GLWidget3D::keyReleaseEvent(QKeyEvent* e) {
	switch (e->key()) {
	case Qt::Key_Control:
		ctrlPressed = false;
		break;
	case Qt::Key_Shift:
		shiftPressed = false;
		break;
	default:
		break;
	}
}

/**
* This function is called once before the first call to paintGL() or resizeGL().
*/
void GLWidget3D::initializeGL() {
	// init glew
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
	}

	if (glewIsSupported("GL_VERSION_4_2"))
		printf("Ready for OpenGL 4.2\n");
	else {
		printf("OpenGL 4.2 not supported\n");
		exit(1);
	}
	const GLubyte* text = glGetString(GL_VERSION);
	printf("VERSION: %s\n", text);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_3D);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glDisable(GL_TEXTURE_3D);

	glEnable(GL_TEXTURE_2D_ARRAY);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glDisable(GL_TEXTURE_2D_ARRAY);

	////////////////////////////////
	renderManager.init("", "", "", true, 8192);
	renderManager.resize(this->width(), this->height());

	glUniform1i(glGetUniformLocation(renderManager.programs["ssao"], "tex0"), 0);//tex0: 0
}

/**
* This function is called whenever the widget has been resized.
*/
void GLWidget3D::resizeGL(int width, int height) {
	height = height ? height : 1;
	glViewport(0, 0, width, height);
	camera.updatePMatrix(width, height);

	renderManager.resize(width, height);
}

/**
* This function is called whenever the widget needs to be painted.
*/
void GLWidget3D::paintEvent(QPaintEvent *event) {
	if (first_paint) {
		std::vector<Vertex> vertices;
		glutils::drawQuad(0.001, 0.001, glm::vec4(1, 1, 1, 1), glm::mat4(), vertices);
		renderManager.addObject("dummy", "", vertices, true);
		renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);
		first_paint = false;
	}

	// draw by OpenGL
	makeCurrent();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	render();

	// unbind texture
	glActiveTexture(GL_TEXTURE0);

	// restore the settings for OpenGL
	glShadeModel(GL_FLAT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	QPainter painter(this);
	painter.setOpacity(1.0f);
	////
	painter.end();

	glEnable(GL_DEPTH_TEST);
}

/**
* This event handler is called when the mouse press events occur.
*/
void GLWidget3D::mousePressEvent(QMouseEvent *e) {
	// This is necessary to get key event occured even after the user selects a menu.
	setFocus();

	if (e->buttons() & Qt::LeftButton) {
		camera.mousePress(e->x(), e->y());
	}
	else if (e->buttons() & Qt::RightButton) {
	}
}

/**
* This event handler is called when the mouse move events occur.
*/

void GLWidget3D::mouseMoveEvent(QMouseEvent *e) {
	if (e->buttons() & Qt::LeftButton) {
		if (shiftPressed) {
			camera.move(e->x(), e->y());
		}
		else {
			camera.rotate(e->x(), e->y(), (ctrlPressed ? 0.1 : 1));
		}
	}

	update();
}

/**
* This event handler is called when the mouse release events occur.
*/
void GLWidget3D::mouseReleaseEvent(QMouseEvent *e) {
}

void GLWidget3D::wheelEvent(QWheelEvent* e) {
	camera.zoom(e->delta() * 0.2);
	update();
}
