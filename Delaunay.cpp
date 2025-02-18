#include <iostream>
#include <list>
#include <vector>
#include "Delaunay.h"

using namespace std;

//this function is called from main, receives a Mesh object
//which containss a std::vector of vertices and another of faces
void triangulateMesh(Mesh &mesh){

	//create temporary triangle that contains all vertices
	//(adds three temporary vertices to mesh)
	superTriangle(mesh);

	//for all vertices in mesh
	for (int i = 0; i < mesh.vertices.size() - 3; i++){
		//add vertex to the delaunay triangulation
		addVertex(mesh, i);
		//record progress
		if (i % 100 == 0) cout << i << " of " << mesh.vertices.size() << " completed.\n";
	}

	//remove any faces that were connected to the temporary super triangle
	for (int i = 0; i < mesh.faces.size(); i++){
		if ((mesh.faces[i].a == mesh.vertices.size() - 3) || (mesh.faces[i].a == mesh.vertices.size() - 2) || (mesh.faces[i].a == mesh.vertices.size() - 1) ||
			(mesh.faces[i].b == mesh.vertices.size() - 3) || (mesh.faces[i].b == mesh.vertices.size() - 2) || (mesh.faces[i].b == mesh.vertices.size() - 1) ||
			(mesh.faces[i].c == mesh.vertices.size() - 3) || (mesh.faces[i].c == mesh.vertices.size() - 2) || (mesh.faces[i].c == mesh.vertices.size() - 1))
		{
			mesh.faces.erase(mesh.faces.begin() + i);
			i = i - 1;
		}
	}
}

//create temporary triangle that contains all vertices
//(adds three temporary vertices to mesh)
void superTriangle(Mesh &mesh){

	double
		xmin = mesh.vertices[0].x,
		xmax = xmin,
		ymin = mesh.vertices[0].y,
		ymax = ymin;

	for (int i = 1; i < mesh.vertices.size(); i++){
		if (mesh.vertices[i].x < xmin) xmin = mesh.vertices[i].x;
		if (mesh.vertices[i].x > xmax) xmax = mesh.vertices[i].x;
		if (mesh.vertices[i].y < ymin) ymin = mesh.vertices[i].y;
		if (mesh.vertices[i].y > ymax) ymax = mesh.vertices[i].y;
	}

	double
		dx = xmax - xmin,
		dy = ymax - ymin,
		dmax = (dx > dy) ? dx : dy,
		xmid = (xmax + xmin) / 2.0,
		ymid = (ymax + ymin) / 2.0;

	Vertex
		v1{ xmid - 20 * dmax, ymid - dmax, 0.0 },
		v2{ xmid + 20 * dmax, ymid - dmax, 0.0 },
		v3{ xmid, ymid + 20 * dmax, 0.0 };

	mesh.vertices.push_back(v1);
	mesh.vertices.push_back(v2);
	mesh.vertices.push_back(v3);
	mesh.faces.push_back(Face{ mesh.vertices.size() - 3, mesh.vertices.size() - 2, mesh.vertices.size() - 1 });

}

//Implement this function
void addVertex(Mesh &mesh, const int vertexIndex){

	vector<Edge> edges_list;

	for (size_t ti = 0; ti < mesh.faces.size(); ti++) {
		Face currentTriangle = mesh.faces[ti];
		//isInsideCircumCircle(const Vertex A, const Vertex B, const Vertex C, const Vertex &point)
		if (isInsideCircumCircle(
			mesh.vertices[currentTriangle.a],
			mesh.vertices[currentTriangle.b],
			mesh.vertices[currentTriangle.c],
			mesh.vertices[vertexIndex])) {

			//We generate the three edges of the fallen triangle
			Edge edgeA;
			Edge edgeB;
			Edge edgeC;

			edgeA.v1 = currentTriangle.a;
			edgeA.v2 = currentTriangle.b;

			edgeB.v1 = currentTriangle.b;
			edgeB.v2 = currentTriangle.c;

			edgeC.v1 = currentTriangle.c;
			edgeC.v2 = currentTriangle.a;

			edges_list.push_back(edgeA);
			edges_list.push_back(edgeB);
			edges_list.push_back(edgeC);

			mesh.faces.erase(mesh.faces.begin() + ti);
			ti--;
		}
	}

	//Lets erase the repeated edges
	for (int i = 0; i < edges_list.size(); i++) {
		for (int j = i+1; j < edges_list.size(); j++) {
			if (edgesCmp(edges_list[i],edges_list[j])) {
				//Delete the the second edge
				edges_list.erase(edges_list.begin() + j);
				edges_list.erase(edges_list.begin() + i);
				i--;
			}
		}
	}


	for (int ei = 0 ; ei < edges_list.size() ; ei++) {
		Face newFace;
		newFace.a = edges_list[ei].v1;
		newFace.b = edges_list[ei].v2;
		newFace.c = vertexIndex;

		mesh.faces.push_back(newFace);
	}

	edges_list.clear();

}

bool edgesCmp(Edge edge1, Edge edge2) {
	bool result = false;

	if (edge1.v1 == edge2.v1 && edge1.v2 == edge2.v2) result = true;
	if (edge1.v1 == edge2.v2 && edge1.v2 == edge2.v1) result = true;

	return result;
}

//returns true if Vertex 'point' is within the circumcircle of triangle ABV
bool isInsideCircumCircle(const Vertex A, const Vertex B, const Vertex C, const Vertex &point){
	double m1, m2, mx1, mx2, my1, my2, xc, yc, r;
	double dx, dy, rsqr, drsqr;

	if (abs(A.y - B.y) < EPSILON && abs(B.y - C.y) < EPSILON)
		return(false);
	if (abs(B.y - A.y) < EPSILON){
		m2 = -(C.x - B.x) / (C.y - B.y);
		mx2 = (B.x + C.x) / 2.0;
		my2 = (B.y + C.y) / 2.0;
		xc = (B.x + A.x) / 2.0;
		yc = m2 * (xc - mx2) + my2;
	}
	else if (abs(C.y - B.y) < EPSILON){
		m1 = -(B.x - A.x) / (B.y - A.y);
		mx1 = (A.x + B.x) / 2.0;
		my1 = (A.y + B.y) / 2.0;
		xc = (C.x + B.x) / 2.0;
		yc = m1 * (xc - mx1) + my1;
	}
	else{
		m1 = -(B.x - A.x) / (B.y - A.y);
		m2 = -(C.x - B.x) / (C.y - B.y);
		mx1 = (A.x + B.x) / 2.0;
		mx2 = (B.x + C.x) / 2.0;
		my1 = (A.y + B.y) / 2.0;
		my2 = (B.y + C.y) / 2.0;
		xc = (m1 * mx1 - m2 * mx2 + my2 - my1) / (m1 - m2);
		yc = m1 * (xc - mx1) + my1;
	}
	dx = B.x - xc;
	dy = B.y - yc;
	rsqr = dx * dx + dy * dy;
	dx = point.x - xc;
	dy = point.y - yc;
	drsqr = dx * dx + dy * dy;
	return((drsqr <= rsqr) ? true : false);

}






