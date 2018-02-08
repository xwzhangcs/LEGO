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
#include "LayerVoxelData.h"
#include "DisjointVoxelData.h"

class MainWindow;

class GLWidget3D : public QGLWidget {
	Q_OBJECT

public:
	enum SHOW_MODE { SHOW_INPUT = 0, SHOW_ALL, SHOW_OPENCV, SHOW_RIGHTANGLE, SHOW_CURVE };
	enum COLOR_MODE { COLOR_SAME = 0, COLOR_BY_BUILDING, COLOR_BY_LAYER };

public:
	MainWindow* mainWin;

	// camera
	Camera camera;
	glm::vec3 light_dir;
	glm::mat4 light_mvpMatrix;
	glm::vec3 spot_light_pos;

	// input voxel data
	util::DisjointVoxelData disjoint_voxel_data;
	//std::vector<std::vector<cv::Mat_<uchar>>> disjointed_voxel_data;
	std::vector<std::shared_ptr<util::Layer>> layers;
	std::vector<std::shared_ptr<simp::Building>> buildings;

	// rendering engine
	RenderManager renderManager;
	int color_mode;
	int show_mode;
	
	// key status
	bool shiftPressed;
	bool ctrlPressed;

	bool first_paint;

public:
	GLWidget3D(MainWindow *parent = 0);

	void drawScene();
	void render();
	void loadVoxelData(const QString& filename);
	void savePLY(const QString& filename);
	void saveImage(const QString& filename);
	void showInputVoxel();
	void simplifyByAll();
	void simplifyByOpenCV(double epsilon, double layering_threshold, double snap_vertex_threshold, double snap_edge_threshold);
	void simplifyByOurCustom(int resolution, double layering_threshold, double snap_vertex_threshold, double snap_edge_threshold);
	void simplifyByCurve(double epsilon, double curve_threshold, double layering_threshold, double snap_vertex_threshold, double snap_edge_threshold);
	void update3DGeometry();
	void update3DGeometry(const std::vector<std::shared_ptr<util::Layer>>& layers);
	void update3DGeometry(std::shared_ptr<util::Layer> layer, const cv::Size& size, glm::vec4& color, std::vector<Vertex>& vertices);
	void update3DGeometry(const std::vector<std::shared_ptr<simp::Building>>& buildings);
	void update3DGeometry(std::shared_ptr<simp::Building> building, glm::vec4& color, std::vector<Vertex>& vertices);

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
