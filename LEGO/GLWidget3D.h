#pragma once

#include <glew.h>
#include "Shader.h"
#include <QGLWidget>
#include <QMouseEvent>
#include "Camera.h"
#include "RenderManager.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Building.h"
#include "GLUtils.h"

class MainWindow;

class GLWidget3D : public QGLWidget {
	Q_OBJECT

public:
	MainWindow* mainWin;

	// camera
	Camera camera;
	glm::vec3 light_dir;
	glm::mat4 light_mvpMatrix;
	glm::vec3 spot_light_pos;

	// input voxel data
	std::vector<cv::Mat> voxel_data;

	// rendering engine
	RenderManager renderManager;
	
	// key status
	bool shiftPressed;
	bool ctrlPressed;

	bool first_paint;

public:
	GLWidget3D(MainWindow *parent = 0);

	void drawScene();
	void render();
	void loadVoxelData(const QString& filename);
	void saveImage(const QString& filename);
	void showInputVoxel();
	void simplifyByOpenCV(double epsilon);
	void simplifyByOurCustom(int resolution);
	double calculateBuildingHeight(const std::vector<cv::Point>& footprint, const std::vector<std::vector<cv::Point>>& holes);
	glm::dvec2 samplePoint(const glutils::BoundingBox& bbox, const std::vector<glm::dvec2>& polygon);
	Building calculateBuildingByOpenCV(const std::vector<cv::Point>& contour, const std::vector<std::vector<cv::Point>>& holes, const QSize& size, double epsilon);
	Building calculateBuildingByOurCustom(const std::vector<cv::Point>& contour, const std::vector<std::vector<cv::Point>>& holes, const QSize& size, int resolution);
	void update3DGeometry(const std::vector<cv::Mat>& voxel_data);
	void update3DGeometry(const std::vector<Building>& buildings);

	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);

protected:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent* e);
};
