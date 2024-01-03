#include "Tower.hpp"

Tower::Tower() : Building(TOWER) {
	AddCircleCoordinates(bottomZ); // alapk�r koordin�t�i
	AddCircleCoordinates(topZ); // bor�t�k�r koordin�t�i

	AddCircleIndexes(1, triangleCount, true); // els� index, ami nem az origo, utols� index, alapk�r-e 
	AddCircleIndexes(triangleCount + 2, 2 * triangleCount + 1, false); // hasonl�an els� index, ami nem az orig�, ez a h�romsz�gek sz�ma + 2 a k�t origo miatt

	AddWallIndexes();
}

Tower::~Tower() {}

void Tower::AddCircleCoordinates(int coordinateZ) {
	meshCPU.vertexArray.push_back({ glm::vec3(0, coordinateZ, 0), glm::vec3(1, 0, 0), glm::vec2(0, 0)});

	// K�r koordin�t�inak kisz�m�t�sa egys�gk�r alapj�n
	for (int i = 0; i < triangleCount; ++i) {
		double colorValue = static_cast<double>(i) / triangleCount;
		meshCPU.vertexArray.push_back(
			{ glm::vec3(cos((360 / triangleCount) * (i + 1) * M_PI / 180) / 2,
						coordinateZ,
						sin((360 / triangleCount) * (i + 1) * M_PI / 180) / 2),
			  glm::vec3(1, 0, 0), 
			  glm::vec2(0, 0) } // sz�ks�ges koordin�t�k meghat�roz�sa az "egys�gk�rben"
		);
	}
}

void Tower::AddCircleIndexes(int from, int to, bool isBaseCircle) {
	// Hozz�adjuk a sz�ks�ges indexeket a t�mbh�z
	for (int i = from; i < to; ++i) {
		meshCPU.indexArray.push_back(from - 1);
		if (isBaseCircle) {
			meshCPU.indexArray.push_back(i + 1);
			meshCPU.indexArray.push_back(i);
		}
		else {
			meshCPU.indexArray.push_back(i);
			meshCPU.indexArray.push_back(i + 1);
		}
	}

	// utols� h�romsz�g kirajzol�sa, ez k�ti �ssze az origot, az utols� pontot �s a legels� pontot egym�ssal
	meshCPU.indexArray.push_back(from - 1);
	if (isBaseCircle) {
		meshCPU.indexArray.push_back(from);
		meshCPU.indexArray.push_back(to);
	}
	else {
		meshCPU.indexArray.push_back(to);
		meshCPU.indexArray.push_back(from);
	}
}

void Tower::AddWallIndexes() {
	// h�r h�romsz�geinek �sszek�t�se minden minden k�rh�romsz�g, kiv�ve az utols� eset�n
	for (int i = 1; i < triangleCount; ++i) {
		meshCPU.indexArray.push_back(i); // als� k�r�n a "bal als�" koordin�na
		meshCPU.indexArray.push_back(triangleCount + i + 2); // fels� k�r�n a "jobb fels�" koordin�ta
		meshCPU.indexArray.push_back(triangleCount + i + 1); // fels� k�r�n a "bal fels�" koordin�ta

		meshCPU.indexArray.push_back(i); // als� k�r "bal als�"
		meshCPU.indexArray.push_back(i + 1); // als� k�r "jobb als�"
		meshCPU.indexArray.push_back(triangleCount + i + 2); // fels� k�r "jobb fels�"
	}

	// utols� h�romsz�gek k�z�tt az egyik h�romsz�g kirajzol�sa: als� utols� koordin�t�ja + fels� els� koordin�t�ja + fels� utols� koordin�t�ja
	meshCPU.indexArray.push_back(triangleCount); // als� "bal als�"
	meshCPU.indexArray.push_back(triangleCount + 2); // fels� "jobb fels�"
	meshCPU.indexArray.push_back(2 * triangleCount + 1); // fels� "bal fels�"

	// szint�n utols� h�romsz�gek k�z�tt a m�sik h�romsz�g: als� utols� + als� els� + fels� els�
	meshCPU.indexArray.push_back(triangleCount); // als� "bal als�"
	meshCPU.indexArray.push_back(1); // als� "jobb als�"
	meshCPU.indexArray.push_back(triangleCount + 2); // fels� "bal fels�"
}

MeshObject<Vertex> Tower::GetMesh() {
	return meshCPU;
}