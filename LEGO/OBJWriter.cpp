#include "OBJWriter.h"
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <QFile>
#include <QTextStream>

bool Material::equals(const Material& other) {
	if (type != other.type) return false;
	if (type == 1) {
		if (color != other.color) return false;
	}
	else if (type == 2) {
		if (texture != other.texture) return false;
	}

	return true;
}

QString Material::to_string() {
	QString ans;
	QTextStream out(&ans);

	if (type == 1) {
		out << "Ka " << color.r << " " << color.g << " " << color.b << endl;
		out << "Kd " << color.r << " " << color.g << " " << color.b << endl;
		out << "Ks " << color.r << " " << color.g << " " << color.b << endl;
		out << "Ke " << color.r << " " << color.g << " " << color.b << endl;
		out << "d 1" << endl;
		out << "Ns 20" << endl;
		out << "illum 2" << endl;
	}
	else if (type == 2) {
		out << "Ka 1 1 1" << endl;
		out << "Kd 1 1 1" << endl;
		out << "Ks 1 1 1" << endl;
		out << "Ke 1 1 1" << endl;
		out << "d 1" << endl;
		out << "Ns 20" << endl;
		out << "illum 2" << endl;
		//out << "map_Ka " << texture << endl;
		out << "map_Kd " << texture << endl;
		//out << "map_Ks " << texture << endl;
		//out << "map_Ns " << texture << endl;
		//out << "map_d " << texture << endl;
	}

	return ans;
}

/*
void OBJWriter::write(const std::vector<sc::SceneObject>& objects, const std::string& filename) {
	std::ofstream file(filename);
	std::ofstream mat_file(filename + ".mtl");

	file << "mtllib " << filename + ".mtl" << std::endl;
	file << std::endl;

	file << "# List of geometric vertices" << std::endl;
	for (int i = 0; i < objects.size(); ++i) {
		for (int j = 0; j < objects[i].faces.size(); ++j) {
			for (int k = 0; k < objects[i].faces[j]->vertices.size(); ++k) {
				file << "v " << objects[i].faces[j]->vertices[k].position.x << " " << objects[i].faces[j]->vertices[k].position.y << " " << objects[i].faces[j]->vertices[k].position.z << std::endl;
			}
		}
	}
	file << std::endl;

	file << "# List of texture coordinates" << std::endl;
	for (int i = 0; i < objects.size(); ++i) {
		for (int j = 0; j < objects[i].faces.size(); ++j) {
			for (int k = 0; k < objects[i].faces[j]->vertices.size(); ++k) {
				if (objects[i].faces[j]->vertices[k].texCoord.x == 0 && objects[i].faces[j]->vertices[k].texCoord.y == 0 && objects[i].faces[j]->vertices[k + 1].texCoord.x == 0 && objects[i].faces[j]->vertices[k + 1].texCoord.y == 0 && objects[i].faces[j]->vertices[k + 2].texCoord.x == 0 && objects[i].faces[j]->vertices[k + 2].texCoord.y == 0) continue;

				file << "vt " << objects[i].faces[j]->vertices[k].texCoord.x << " " << objects[i].faces[j]->vertices[k].texCoord.y << std::endl;
			}
		}
	}
	file << std::endl;

	file << "# List of vertex normals" << std::endl;
	for (int i = 0; i < objects.size(); ++i) {
		for (int j = 0; j < objects[i].faces.size(); ++j) {
			for (int k = 0; k < objects[i].faces[j]->vertices.size(); ++k) {
				file << "vn " << objects[i].faces[j]->vertices[k].normal.x << " " << objects[i].faces[j]->vertices[k].normal.y << " " << objects[i].faces[j]->vertices[k].normal.z << std::endl;
			}
		}
	}
	file << std::endl;


	int positionId = 1;
	int texCoordId = 1;
	int materialId = 1;
	Material material;
	for (int i = 0; i < objects.size(); ++i) {
		for (int j = 0; j < objects[i].faces.size(); ++j) {
			bool textureEnabled = true;
			if (objects[i].faces[j]->texture.empty()) {
				textureEnabled = false;
			}
			if (objects[i].faces[j]->vertices[0].texCoord.x == 0 && objects[i].faces[j]->vertices[0].texCoord.y == 0 && objects[i].faces[j]->vertices[1].texCoord.x == 0 && objects[i].faces[j]->vertices[1].texCoord.y == 0 && objects[i].faces[j]->vertices[2].texCoord.x == 0 && objects[i].faces[j]->vertices[2].texCoord.y == 0) {
				textureEnabled = false;
			}

			Material new_material;
			if (textureEnabled) {
				new_material = Material(objects[i].faces[j]->texture);
			}
			else {
				new_material = Material(objects[i].faces[j]->vertices[0].color);
			}

			if (!new_material.equals(material)) {
				material = new_material;

				mat_file << "newmtl Material" << materialId << std::endl;
				mat_file << material.to_string() << std::endl;

				file << std::endl;
				file << "usemtl Material" << materialId << std::endl;
				materialId++;
			}

			for (int k = 0; k < objects[i].faces[j]->vertices.size() / 3; ++k) {
				file << "f ";
				for (int l = 0; l < 3; ++l) {
					if (l > 0) {
						file << " ";
					}
					file << positionId;

					file << "/";
					if (objects[i].faces[j]->vertices[k].texCoord.x != 0 || objects[i].faces[j]->vertices[k].texCoord.y != 0 || objects[i].faces[j]->vertices[k + 1].texCoord.x != 0 || objects[i].faces[j]->vertices[k + 1].texCoord.y != 0 || objects[i].faces[j]->vertices[k + 2].texCoord.x != 0 || objects[i].faces[j]->vertices[k + 2].texCoord.y != 0) {
						file << texCoordId++;
					}

					file << "/" << positionId++;
				}
				file << std::endl;
			}
		}
	}

	file.close();
}*/

/*
void OBJWriter::write(const std::vector<boost::shared_ptr<glutils::Face> >& faces, const std::string& filename) {
	std::ofstream file(filename);
	std::ofstream mat_file(filename + ".mtl");

	boost::filesystem::path p(filename + ".mtl");

	file << "mtllib " << p.filename().string() << std::endl;
	file << std::endl;

	file << "# List of geometric vertices" << std::endl;
	for (int j = 0; j < faces.size(); ++j) {
		for (int k = 0; k < faces[j]->vertices.size(); ++k) {
			file << "v " << faces[j]->vertices[k].position.x << " " << faces[j]->vertices[k].position.y << " " << faces[j]->vertices[k].position.z << std::endl;
		}
	}
	file << std::endl;

	file << "# List of texture coordinates" << std::endl;
	for (int j = 0; j < faces.size(); ++j) {
		if (faces[j]->vertices.size() < 3) continue;
		if (faces[j]->vertices[0].texCoord.x == 0 && faces[j]->vertices[0].texCoord.y == 0 && faces[j]->vertices[1].texCoord.x == 0 && faces[j]->vertices[1].texCoord.y == 0 && faces[j]->vertices[2].texCoord.x == 0 && faces[j]->vertices[2].texCoord.y == 0) continue;

		for (int k = 0; k < faces[j]->vertices.size(); ++k) {
			file << "vt " << faces[j]->vertices[k].texCoord.x << " " << faces[j]->vertices[k].texCoord.y << std::endl;
		}
	}
	file << std::endl;

	file << "# List of vertex normals" << std::endl;
	for (int j = 0; j < faces.size(); ++j) {
		for (int k = 0; k < faces[j]->vertices.size(); ++k) {
			file << "vn " << faces[j]->vertices[k].normal.x << " " << faces[j]->vertices[k].normal.y << " " << faces[j]->vertices[k].normal.z << std::endl;
		}
	}
	file << std::endl;


	int positionId = 1;
	int texCoordId = 1;
	int materialId = 1;
	Material material;
	for (int j = 0; j < faces.size(); ++j) {
		if (faces[j]->vertices.size() < 3) continue;

		bool textureEnabled = true;
		if (faces[j]->texture.empty()) {
			textureEnabled = false;
		}
		if (faces[j]->vertices[0].texCoord.x == 0 && faces[j]->vertices[0].texCoord.y == 0 && faces[j]->vertices[1].texCoord.x == 0 && faces[j]->vertices[1].texCoord.y == 0 && faces[j]->vertices[2].texCoord.x == 0 && faces[j]->vertices[2].texCoord.y == 0) {
			textureEnabled = false;
		}

		Material new_material;
		if (textureEnabled) {
			new_material = Material(faces[j]->texture);
		}
		else {
			new_material = Material(faces[j]->vertices[0].color);
		}

		if (!new_material.equals(material)) {
			material = new_material;

			mat_file << "newmtl Material" << materialId << std::endl;
			mat_file << material.to_string() << std::endl;

			file << std::endl;
			file << "usemtl Material" << materialId << std::endl;
			materialId++;
		}

		for (int k = 0; k < faces[j]->vertices.size() / 3; ++k) {
			file << "f ";
			for (int l = 0; l < 3; ++l) {
				if (l > 0) {
					file << " ";
				}
				file << positionId;

				file << "/";
				if (faces[j]->vertices[k].texCoord.x != 0 || faces[j]->vertices[k].texCoord.y != 0 || faces[j]->vertices[k + 1].texCoord.x != 0 || faces[j]->vertices[k + 1].texCoord.y != 0 || faces[j]->vertices[k + 2].texCoord.x != 0 || faces[j]->vertices[k + 2].texCoord.y != 0) {
					file << texCoordId++;
				}

				file << "/" << positionId++;
			}
			file << std::endl;
		}
	}

	file.close();
}
*/

void OBJWriter::write(const std::vector<Vertex>& vertices, const QString& filename) {
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly)) return;

	QTextStream out(&file);

	out << "# List of geometric vertices\n";
	for (int i = 0; i < vertices.size(); i+=3) {
		for (int j = 0; j < 3; j++) {
			out << "v " << vertices[i + j].position.x << " " << vertices[i + j].position.y << " " << vertices[i + j].position.z << "\n";
		}
	}
	out << "\n";

	out << "# List of vertex normals\n";
	for (int i = 0; i < vertices.size(); i+=3) {
		for (int j = 0; j < 3; j++) {
			out << "vn " << vertices[i + j].normal.x << " " << vertices[i + j].normal.y << " " << vertices[i + j].normal.z << "\n";
		}
	}
	out << "\n";

	out << "# Polygonal face element\n";
	for (int i = 0; i < vertices.size(); i += 3) {
		out << "f " << i + 1 << "//" << i + 1 << " " << i + 2 << "//" << i + 2 << " " << i + 3 << "//" << i + 3 << "\n";
	}

	file.close();

}