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
#include "ContourUtils.h"

GLWidget3D::GLWidget3D(MainWindow *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers)) {
	this->mainWin = parent;
	ctrlPressed = false;
	shiftPressed = false;

	first_paint = true;

	// This is necessary to prevent the screen overdrawn by OpenGL
	setAutoFillBackground(false);

	// light direction for shadow mapping
	light_dir = glm::normalize(glm::vec3(-4, -5, -8));

	// model/view/projection matrices for shadow mapping
	glm::mat4 light_pMatrix = glm::ortho<float>(-200, 200, -200, 200, 0.1, 400);
	glm::mat4 light_mvMatrix = glm::lookAt(-light_dir * 50.0f, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	light_mvpMatrix = light_pMatrix * light_mvMatrix;

	// spot light
	spot_light_pos = glm::vec3(2, 2.5, 8);
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
	voxel_data.resize(files.size());
	for (int i = 0; i < files.size(); i++) {
		voxel_data[i] = cv::imread((dir.absolutePath() + "/" + files[i]).toUtf8().constData(), cv::IMREAD_GRAYSCALE);
	}

	update3DGeometry(voxel_data);
}

void GLWidget3D::saveImage(const QString& filename) {
	QImage image = grabFrameBuffer();
	image.save(filename);
}

void GLWidget3D::showInputVoxel() {
	update3DGeometry(voxel_data);
}

void GLWidget3D::simplifyByOpenCV(double epsilon, double slicing_threshold) {
	std::vector<Building> buildings;

	// get size
	QSize size(voxel_data[0].cols, voxel_data[0].rows);

	// extract contours
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(voxel_data[5].clone(), contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	// traverse all the external contours
	int cnt = 0;
	for (int i = 0; i < hierarchy.size(); i++) {
		if (hierarchy[i][3] != -1) continue;
		
		// obtain all the holes inside this contour
		std::vector<std::vector<cv::Point>> holes;
		int hole_id = hierarchy[i][2];
		while (hole_id != -1) {
			holes.push_back(contours[hole_id]);

			hole_id = hierarchy[hole_id][0];
		}
		
		std::vector<cv::Point> contour = contours[i];

		calculateBuildingLayers(contour, holes, 5, size, epsilon, slicing_threshold, buildings);
	}

	update3DGeometry(buildings);
}

void GLWidget3D::calculateBuildingLayers(std::vector<cv::Point> contour, std::vector<std::vector<cv::Point>> holes, int height, const QSize& size, double epsilon, double slicing_threshold, std::vector<Building>& buildings) {
	// calculate the bounding box
	int min_x = std::numeric_limits<int>::max();
	int max_x = -std::numeric_limits<int>::max();
	int min_y = std::numeric_limits<int>::max();
	int max_y = -std::numeric_limits<int>::max();
	for (int i = 0; i < contour.size(); i++) {
		min_x = std::min(min_x, contour[i].x);
		max_x = std::max(max_x, contour[i].x);
		min_y = std::min(min_y, contour[i].y);
		max_y = std::max(max_y, contour[i].y);
	}

	// find the height at which the contour drastically changes
	int next_height = findDrasticChange(voxel_data, height, contour, holes, slicing_threshold);

	// calculate building by simplifying the contour and holes
	try {
		Building building = calculateBuildingByOpenCV(contour, holes, size, height, next_height, epsilon);
		buildings.push_back(building);
		
		if (next_height >= voxel_data.size()) return;

		cv::Mat cropped_img(voxel_data[next_height], cv::Rect(min_x, min_y, max_x - min_x + 1, max_y - min_y + 1));
		cv::imwrite("test.png", cropped_img);

		// extract contours
		std::vector<std::vector<cv::Point>> contours_in_copped_img;
		std::vector<cv::Vec4i> hierarchy_in_cropped_img;
		cv::findContours(cropped_img, contours_in_copped_img, hierarchy_in_cropped_img, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

		bool found_next_contour = false;
		for (int i = 0; i < hierarchy_in_cropped_img.size(); i++) {
			if (hierarchy_in_cropped_img[i][3] != -1) continue;

			found_next_contour = true;
			contour = contours_in_copped_img[i];

			// obtain all the holes inside this contour
			holes.clear();
			int hole_id = hierarchy_in_cropped_img[i][2];
			while (hole_id != -1) {
				holes.push_back(contours_in_copped_img[hole_id]);
				hole_id = hierarchy_in_cropped_img[hole_id][0];
			}

			// offset back the contour and holes
			for (int i = 0; i < contour.size(); i++) {
				contour[i].x += min_x;
				contour[i].y += min_y;
			}
			for (int i = 0; i < holes.size(); i++) {
				for (int j = 0; j < holes[i].size(); j++) {
					holes[i][j].x += min_x;
					holes[i][j].y += min_y;
				}
			}

			calculateBuildingLayers(contour, holes, next_height, size, epsilon, slicing_threshold, buildings);
		}
	}
	catch (char* ex) {
	}
}

/**
* Calculate the building geometry by simplifying the specified footprint and holes using OpenCV function.
*/
Building GLWidget3D::calculateBuildingByOpenCV(const std::vector<cv::Point>& contour, const std::vector<std::vector<cv::Point>>& holes, const QSize& size, int bottom_height, int top_height, double epsilon) {
	std::vector<cv::Point> simplified_contour;
	cv::approxPolyDP(contour, simplified_contour, epsilon, true);

	if (simplified_contour.size() < 3) throw "Invalid contour";

	// create a building object
	Building building;
	building.bottom_height = bottom_height;
	building.top_height = top_height;

	building.footprint.resize(simplified_contour.size());
	for (int i = 0; i < simplified_contour.size(); i++) {
		building.footprint[i] = glm::dvec2(simplified_contour[i].x - size.width() * 0.5, size.height() * 0.5 - simplified_contour[i].y);
	}

	// simplify the hole as well
	building.holes.resize(holes.size());
	for (int i = 0; i < holes.size(); i++) {
		std::vector<cv::Point2f> simplified_hole;
		cv::approxPolyDP(holes[i], simplified_hole, epsilon, true);
		if (simplified_hole.size() >= 3) {
			building.holes[i].resize(simplified_hole.size());
			for (int j = 0; j < simplified_hole.size(); j++) {
				building.holes[i][j] = glm::dvec2(simplified_hole[j].x - size.width() * 0.5, size.height() * 0.5 - simplified_hole[j].y);
			}
		}
	}

	return building;
}

void GLWidget3D::simplifyByOurCustom(int resolution) {
	std::vector<Building> buildings;

	// get size
	QSize size(voxel_data[0].cols, voxel_data[0].rows);

	// extract contours
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(voxel_data[5].clone(), contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	// traverse all the external contours
	int cnt = 0;
	for (int i = 0; i < hierarchy.size(); i++) {
		std::cout << "Processing building " << i << std::endl;

		if (hierarchy[i][3] != -1) continue;
		if (contours[i].size() < 3) continue;

		// obtain all the holes inside this contour
		std::vector<std::vector<cv::Point>> holes;
		int hole_id = hierarchy[i][2];
		while (hole_id != -1) {
			holes.push_back(contours[hole_id]);

			hole_id = hierarchy[hole_id][0];
		}

		// calculate building by simplifying the contour and holes
		try {
			Building building = calculateBuildingByOurCustom(contours[i], holes, size, resolution);
			buildings.push_back(building);
			cnt++;
		}
		catch (char* ex) {
		}
	}
	std::cout << "Processing buildings has been finished." << std::endl;

	update3DGeometry(buildings);
}

/**
* Calculate the building geometry by simplifying the specified footprint and holes using OpenCV function.
*/
Building GLWidget3D::calculateBuildingByOurCustom(const std::vector<cv::Point>& contour, const std::vector<std::vector<cv::Point>>& holes, const QSize& size, int resolution) {
	std::vector<cv::Point2f> simplified_contour;
	contour::simplify(contour, simplified_contour, resolution);

	if (simplified_contour.size() < 3) throw "Invalid contour";

	// create a building object
	Building building;
	building.bottom_height = 0;
	building.top_height = calculateBuildingHeight(contour, holes);

	building.footprint.resize(simplified_contour.size());
	for (int i = 0; i < simplified_contour.size(); i++) {
		building.footprint[i] = glm::dvec2(simplified_contour[i].x - size.width() * 0.5, size.height() * 0.5 - simplified_contour[i].y);
	}

	// simplify the hole as well
	building.holes.resize(holes.size());
	for (int i = 0; i < holes.size(); i++) {
		std::vector<cv::Point2f> simplified_hole;
		std::vector<cv::Point2f> simplified_hole2;
		contour::simplify(holes[i], simplified_hole, resolution);
		if (simplified_hole.size() >= 3) {
			building.holes[i].resize(simplified_hole.size());
			for (int j = 0; j < simplified_hole.size(); j++) {
				building.holes[i][j] = glm::dvec2(simplified_hole[j].x - size.width() * 0.5, size.height() * 0.5 - simplified_hole[j].y);
			}
		}
	}

	return building;
}

/**
 * Calculate the building height for the specified footprint and holes.
 * Sample many points within the footprint excluding the holes, and check the maximum height such that 65% of the sampled points are still remained.
 *
 * @param footprint		footprint of the building
 * @param holes			holes of the building
 * @return				height of the building
 */
double GLWidget3D::calculateBuildingHeight(const std::vector<cv::Point>& footprint, const std::vector<std::vector<cv::Point>>& holes) {
	std::vector<glm::dvec2> footprint2(footprint.size());
	for (int i = 0; i < footprint.size(); i++) {
		footprint2[i] = glm::dvec2(footprint[i].x, footprint[i].y);
	}
	std::vector<std::vector<glm::dvec2>> holes2(holes.size());
	for (int i = 0; i < holes.size(); i++) {
		holes2[i].resize(holes[i].size());
		for (int j = 0; j < holes[i].size(); j++) {
			holes2[i][j] = glm::dvec2(holes[i][j].x, holes[i][j].y);
		}
	}

	// calculate bounding box
	glutils::BoundingBox bbox(footprint2);

	// sample points
	const int N = 30;
	std::vector<glm::dvec2> points(N);
	for (int i = 0; i < N; i++) {
		glm::dvec2 pt;
		while (true) {
			pt = samplePoint(bbox, footprint2);
			bool inside = true;
			for (int j = 0; j < holes.size(); j++) {
				if (glutils::isWithinPolygon(pt, holes2[j])) {
					inside = false;
					break;
				}
			}
			if (inside) break;
		}

		points[i] = pt;
	}

	// check the height of points
	for (int i = 5; i < voxel_data.size(); i++) {
		int cnt = 0;
		for (int j = 0; j < points.size(); j++) {
			int u = (int)points[j].x;
			int v = (int)points[j].y;
			u = std::min(std::max(0, u), voxel_data[i].cols - 1);
			v = std::min(std::max(0, v), voxel_data[i].rows - 1);

			if (voxel_data[i].at<uchar>(v, u) == 255) cnt++;
		}

		if (cnt < points.size() * 0.65) {
			return i;
		}
	}

	return voxel_data.size();
}

glm::dvec2 GLWidget3D::samplePoint(const glutils::BoundingBox& bbox, const std::vector<glm::dvec2>& polygon) {
	for (int iter = 0; iter < 10000; iter++) {
		glm::dvec2 pt(bbox.minPt.x + bbox.sx() * (double)(rand() % 1000) / 999.0, bbox.minPt.y + bbox.sy() * (double)(rand() % 1000) / 999.0);
		if (glutils::isWithinPolygon(pt, polygon)) return pt;
	}

	return glm::dvec2(bbox.center());
}

int GLWidget3D::findDrasticChange(const std::vector<cv::Mat>& voxel_data, int start_id, const std::vector<cv::Point>& contour, const std::vector<std::vector<cv::Point>>& holes, double threshold) {
	// calculate the bounding box
	int min_x = std::numeric_limits<int>::max();
	int max_x = -std::numeric_limits<int>::max();
	int min_y = std::numeric_limits<int>::max();
	int max_y = -std::numeric_limits<int>::max();
	for (int i = 0; i < contour.size(); i++) {
		min_x = std::min(min_x, contour[i].x);
		max_x = std::max(max_x, contour[i].x);
		min_y = std::min(min_y, contour[i].y);
		max_y = std::max(max_y, contour[i].y);
	}

	// create image of the contour of the current slice
	cv::Mat img(voxel_data[0].rows, voxel_data[0].cols, CV_8U, cv::Scalar(0));
	std::vector<std::vector<cv::Point>> contours(1);
	contours[0] = contour;
	cv::fillPoly(img, contours, cv::Scalar(255), cv::LINE_4);
	for (int i = 0; i < holes.size(); i++) {
		std::vector<std::vector<cv::Point>> hole_pts(1);
		hole_pts[0] = holes[i];
		cv::fillPoly(img, hole_pts, cv::Scalar(128), cv::LINE_4);
	}

	int union_cnt = 0;
	int inter_cnt = 0;
	for (int i = start_id + 1; i < voxel_data.size(); i++) {
		for (int r = min_y; r <= max_y; r++) {
			for (int c = min_x; c <= max_x; c++) {
				// ignore the pixels within the holes
				if (img.at<uchar>(r, c) == 128) continue;

				if (img.at<uchar>(r, c) == 255 || voxel_data[i].at<uchar>(r, c) == 255) union_cnt++;
				if (img.at<uchar>(r, c) == 255 && voxel_data[i].at<uchar>(r, c) == 255) inter_cnt++;
			}
		}

		if ((float)inter_cnt / union_cnt < threshold) {
			return i;
		}
	}

	return voxel_data.size();
}

void GLWidget3D::update3DGeometry(const std::vector<cv::Mat>& voxel_data) {
	renderManager.removeObjects();

	QSize size(voxel_data[0].cols, voxel_data[0].rows);

	std::vector<Vertex> vertices;
	for (int i = 0; i < voxel_data.size(); i++) {
		for (int y = 0; y < voxel_data[i].rows; y++) {
			for (int x = 0; x < voxel_data[i].cols; x++) {
				if (voxel_data[i].at<uchar>(y, x) != 255) continue;
				glutils::drawBox(1, 1, 1, glm::vec4(0.7, 1, 0.7, 1), glm::translate(glm::rotate(glm::mat4(), -(float)glutils::M_PI * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(x + 0.5 - size.width() * 0.5, size.height() * 0.5 - y - 0.5, i + 0.5)), vertices);
			}
		}
	}

	renderManager.addObject("building", "", vertices, true);

	// update shadow map
	renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);
}

void GLWidget3D::update3DGeometry(const std::vector<Building>& buildings) {
	renderManager.removeObjects();

	std::vector<Vertex> vertices;
	for (int i = 0; i < buildings.size(); i++) {
		std::cout << "generate geometry " << i << std::endl;

		if (buildings[i].holes.size() == 0) {
			glutils::drawPrism(buildings[i].footprint, buildings[i].top_height - buildings[i].bottom_height, glm::vec4(0.7, 1, 0.7, 1), glm::translate(glm::rotate(glm::mat4(), -(float)glutils::M_PI * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, buildings[i].bottom_height)), vertices);
		}
		else {
			glutils::drawPrismWithHoles(buildings[i].footprint, buildings[i].holes, buildings[i].top_height - buildings[i].bottom_height, glm::vec4(0.7, 1, 0.7, 1), glm::translate(glm::rotate(glm::mat4(), -(float)glutils::M_PI * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(0, 0, buildings[i].bottom_height)), vertices);
		}
	}

	renderManager.addObject("building", "", vertices, true);

	// update shadow map
	renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);
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
	}
	else if (e->buttons() & Qt::RightButton) {
		camera.mousePress(e->x(), e->y());
	}
}

/**
* This event handler is called when the mouse move events occur.
*/

void GLWidget3D::mouseMoveEvent(QMouseEvent *e) {
	if (e->buttons() & Qt::RightButton) {
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
