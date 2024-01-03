#include "Tower.hpp"

Tower::Tower() : Building(TOWER) {
	AddCircleCoordinates(bottomZ); // alapkör koordinátái
	AddCircleCoordinates(topZ); // borítókör koordinátái

	AddCircleIndexes(1, triangleCount, true); // elsõ index, ami nem az origo, utolsó index, alapkör-e 
	AddCircleIndexes(triangleCount + 2, 2 * triangleCount + 1, false); // hasonlóan elsõ index, ami nem az origó, ez a háromszögek száma + 2 a két origo miatt

	AddWallIndexes();
}

Tower::~Tower() {}

void Tower::AddCircleCoordinates(int coordinateZ) {
	meshCPU.vertexArray.push_back({ glm::vec3(0, coordinateZ, 0), glm::vec3(1, 0, 0), glm::vec2(0, 0)});

	// Kör koordinátáinak kiszámítása egységkör alapján
	for (int i = 0; i < triangleCount; ++i) {
		double colorValue = static_cast<double>(i) / triangleCount;
		meshCPU.vertexArray.push_back(
			{ glm::vec3(cos((360 / triangleCount) * (i + 1) * M_PI / 180) / 2,
						coordinateZ,
						sin((360 / triangleCount) * (i + 1) * M_PI / 180) / 2),
			  glm::vec3(1, 0, 0), 
			  glm::vec2(0, 0) } // szükséges koordináták meghatározása az "egységkörben"
		);
	}
}

void Tower::AddCircleIndexes(int from, int to, bool isBaseCircle) {
	// Hozzáadjuk a szükséges indexeket a tömbhöz
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

	// utolsó háromszög kirajzolása, ez köti össze az origot, az utolsó pontot és a legelsõ pontot egymással
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
	// hör háromszögeinek összekötése minden minden körháromszög, kivéve az utolsó esetén
	for (int i = 1; i < triangleCount; ++i) {
		meshCPU.indexArray.push_back(i); // alsó körön a "bal alsó" koordinána
		meshCPU.indexArray.push_back(triangleCount + i + 2); // felsõ körön a "jobb felsõ" koordináta
		meshCPU.indexArray.push_back(triangleCount + i + 1); // felsõ körön a "bal felsõ" koordináta

		meshCPU.indexArray.push_back(i); // alsó kör "bal alsó"
		meshCPU.indexArray.push_back(i + 1); // alsó kör "jobb alsó"
		meshCPU.indexArray.push_back(triangleCount + i + 2); // felsõ kör "jobb felsõ"
	}

	// utolsó háromszögek között az egyik háromszög kirajzolása: alsó utolsó koordinátája + felsõ elsõ koordinátája + felsõ utolsó koordinátája
	meshCPU.indexArray.push_back(triangleCount); // alsó "bal alsó"
	meshCPU.indexArray.push_back(triangleCount + 2); // felsõ "jobb felsõ"
	meshCPU.indexArray.push_back(2 * triangleCount + 1); // felsõ "bal felsõ"

	// szintén utolsó háromszögek között a másik háromszög: alsõ utolsó + alsó elsõ + felsõ elsõ
	meshCPU.indexArray.push_back(triangleCount); // alsó "bal alsó"
	meshCPU.indexArray.push_back(1); // alsó "jobb alsó"
	meshCPU.indexArray.push_back(triangleCount + 2); // felsõ "bal felsõ"
}

MeshObject<Vertex> Tower::GetMesh() {
	return meshCPU;
}