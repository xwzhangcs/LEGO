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

	std::vector<cv::Mat> voxel_data;

	// rendering engine
	RenderManager renderManager;
	
	std::vector<Building> buildings;

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
	void inputVoxel();
	void simplifyByOpenCV(double epsilon);
	void simplifyByOurCustom();
	double calculateBuildingHeight(const std::vector<cv::Mat>& voxel_data, const std::vector<glm::dvec2>& footprint, const std::vector<std::vector<glm::dvec2>>& holes);
	glm::dvec2 samplePoint(const glutils::BoundingBox& bbox, const std::vector<glm::dvec2>& polygon);
	Building calculateBuilding(std::vector<std::vector<cv::Point>> contours, std::vector<cv::Vec4i> hierarchy, int i, const QSize& size, double epsilon);
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
