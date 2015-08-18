
/*
 val3dity - Copyright (c) 2011-2015, Hugo Ledoux.  All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of the authors nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL HUGO LEDOUX BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
*/

#include "validate_shell.h"


//-- CGAL stuff
typedef CgalPolyhedron::HalfedgeDS              HalfedgeDS;
typedef CGAL::Bbox_3                            Bbox;
typedef K::Segment_3                            Segment;
typedef CgalPolyhedron::Halfedge_const_handle   Halfedge_const_handle;
typedef CgalPolyhedron::Facet_const_iterator    Facet_const_iterator;
typedef CgalPolyhedron::Facet_const_handle      Facet_const_handle;


CgalPolyhedron* construct_CgalPolyhedron_incremental(vector< vector<int*> > *lsTr, vector<Point3> *lsPts, Shell2* sh)
{
  CgalPolyhedron* P = new CgalPolyhedron();
  ConstructShell<HalfedgeDS> s(lsTr, lsPts, sh);
  if (s.isValid)
    P->delegate(s);
  else
  {
    delete P;
    P = NULL;
  }
  return P;
}

template <class HDS>
void ConstructShell<HDS>::operator()( HDS& hds) 
{
  typedef typename HDS::Vertex          Vertex;
  typedef typename Vertex::Point        Point;
  typedef typename HDS::Face_handle     FaceH;
  typedef typename HDS::Halfedge_handle heH;
  CGAL::Polyhedron_incremental_builder_3<HDS> B(hds, false);
  B.begin_surface((*lsPts).size(), (*faces).size());
  vector<Point3>::const_iterator itPt = lsPts->begin();
  for ( ; itPt != lsPts->end(); itPt++)
  { 
    B.add_vertex( Point(itPt->x(), itPt->y(), itPt->z()));
  }
  construct_faces_order_given(B);
  if (isValid)
  {
    if (B.check_unconnected_vertices() == true) {
      sh->add_error(309, -1, "");
      B.remove_unconnected_vertices();
    }
  }
  B.end_surface();
}

template <class HDS>
void ConstructShell<HDS>::construct_faces_order_given(CGAL::Polyhedron_incremental_builder_3<HDS>& B)
{
  vector< vector<int*> >::const_iterator itF = faces->begin();
  int faceID = 0;
  for ( ; itF != faces->end(); itF++)
  {
    vector<int*>::const_iterator itF2 = itF->begin();
    for ( ; itF2 != itF->end(); itF2++)
    {
      int* a = *itF2;
      add_one_face(B, a[0], a[1], a[2], faceID);
    }
    faceID++;
  }
}

//  void construct_faces_keep_adjcent(CGAL::Polyhedron_incremental_builder_3<HDS>& B, cbf cb)
//  {
//    //-- build a 2D-matrix of usage for each edge
//    int size = static_cast<int>((*lsPts).size());
//    bool halfedges[size*size];
//    for (int i = 0; i <= (size*size); i++)
//      halfedges[i] = false;
//    
//    //-- start with the first face (not only 1st triangle, first original face)
//    vector< vector<int*> >::const_iterator itF = faces->begin();
//    vector<int*>::const_iterator itF2 = itF->begin();
//    for ( ; itF2 != itF->end(); itF2++)
//    {
//      int* a = *itF2;
//      std::vector< std::size_t> faceids(3);        
//      faceids[0] = a[0];
//      faceids[1] = a[1];
//      faceids[2] = a[2];
//      B.add_facet(faceids.begin(), faceids.end());
//      halfedges[m2a(a[0], a[1])] = true;
//      halfedges[m2a(a[1], a[2])] = true;
//      halfedges[m2a(a[2], a[0])] = true;
//    }
//    itF++;
//
//    //-- process the other faces
//    int faceID = 0;
//    for ( ; itF != faces->end(); itF++)
//    {
//      itF2 = itF->begin();
//      for ( ; itF2 != itF->end(); itF2++)
//      {
//        int* a = *itF2;
//        std::vector< std::size_t> faceids(3);        
//        faceids[0] = a[0];
//        faceids[1] = a[1];
//        faceids[2] = a[2];
//        if ( (B.test_facet(faceids.begin(), faceids.end()) ==  true) && (is_connected(a, halfedges) == true) )
//        {
//          B.add_facet(faceids.begin(), faceids.end());
//          halfedges[m2a(a[0], a[1])] = true;
//          halfedges[m2a(a[1], a[2])] = true;
//          halfedges[m2a(a[2], a[0])] = true;
//        }
//      }
//      faceID++;
//    }
//    
//    /////////////////////////////////////////////////
//    
//
//    while (trFaces.size() > 0)
//    {
//      bool success = false;
//      for (list<int*>::iterator it1 = trFaces.begin(); it1 != trFaces.end(); it1++)
//      {
//        int* a = *it1;
//        std::vector< std::size_t> faceids(3);        
//        faceids[0] = a[0];
//        faceids[1] = a[1];
//        faceids[2] = a[2];
////        cout << B.test_facet(faceids.begin(), faceids.end()) << endl;
////        cout << is_connected(a, halfedges) << endl;
//        if ( (B.test_facet(faceids.begin(), faceids.end()) ==  true) && (is_connected(a, halfedges) == true) )
//        {
//          B.add_facet(faceids.begin(), faceids.end());
//          cout << "face added\n";
//          halfedges[m2a(a[0],a[1])] = true;
//          halfedges[m2a(a[1],a[2])] = true;
//          halfedges[m2a(a[2],a[0])] = true;
//          trFaces.erase(it1);
//          success = true;
//          break;
//        }
//      }
//      if (success == false)
//      {
//        (*cb)(304, shellID, 0, "");       
//      }
//    }
//  }

template <class HDS>
int ConstructShell<HDS>::m2a(int m, int n)
{
  return (m+(n*_width));
}

template <class HDS>
void ConstructShell<HDS>::construct_faces_flip_when_possible(CGAL::Polyhedron_incremental_builder_3<HDS>& B)
{
  int size = static_cast<int>((*lsPts).size());
#ifdef WIN32
  bool *halfedges = new bool[size*size];
#else
  bool halfedges[size*size];
#endif
  for (int i = 0; i <= (size*size); i++)
    halfedges[i] = false;
  
  //-- build one flat list of the triangular faces, for convenience
  list<int*> trFaces;
  vector< vector<int*> >::const_iterator itF = faces->begin();
  for ( ; itF != faces->end(); itF++)
  {
    vector<int*>::const_iterator itF2 = itF->begin();
    for ( ; itF2 != itF->end(); itF2++)
    {
      int* a = *itF2;
      trFaces.push_back(a);
    }
  }
  //-- start with the first one
  int* a = trFaces.front();
  std::vector< std::size_t> faceids(3);        
  faceids[0] = a[0];
  faceids[1] = a[1];
  faceids[2] = a[2];
  B.add_facet(faceids.begin(), faceids.end());

  halfedges[m2a(a[0], a[1])] = true;
  halfedges[m2a(a[1], a[2])] = true;
  halfedges[m2a(a[2], a[0])] = true;

  trFaces.pop_front();
  while (trFaces.size() > 0)
  {
#ifdef WIN32
     if (try_to_add_face(B, trFaces, halfedges, true) == false)
#else
     if (try_to_add_face(B, trFaces, &halfedges[0], true) == false)
#endif
    {
      //-- add first face possible, will be dangling by definition
      //cout << "had problems..." << endl;
#ifdef WIN32
       if (try_to_add_face(B, trFaces, halfedges, false) == false)
#else
       if (try_to_add_face(B, trFaces, &halfedges[0], false) == false)
#endif
      {
        //-- cannot repair. non-manifold situations.
        trFaces.clear();
      }
           
    }
  }
#ifdef WIN32
  delete [] halfedges; halfedges = NULL;
#endif
}


template <class HDS>
bool ConstructShell<HDS>::try_to_add_face(CGAL::Polyhedron_incremental_builder_3<HDS>& B, list<int*>& trFaces, bool* halfedges, bool bMustBeConnected)
{
  bool success = false;
  for (list<int*>::iterator it1 = trFaces.begin(); it1 != trFaces.end(); it1++)
  {
    int* a = *it1;
    std::vector< std::size_t> faceids(3);        
    faceids[0] = a[0];
    faceids[1] = a[1];
    faceids[2] = a[2];
    bool test = B.test_facet(faceids.begin(), faceids.end());
    if (test == true)
    {
      if ( (bMustBeConnected == false) || (is_connected(a, halfedges) == true) )
      {
        B.add_facet(faceids.begin(), faceids.end());
        halfedges[m2a(a[0],a[1])] = true;
        halfedges[m2a(a[1],a[2])] = true;
        halfedges[m2a(a[2],a[0])] = true;
        trFaces.erase(it1);
        success = true;
        break;
      }
    }
    else //-- face conflicts with one, try to flip it and insert if possible
    {
      faceids[1] = a[2];
      faceids[2] = a[1];
      if (B.test_facet(faceids.begin(), faceids.end()))
      {
        B.add_facet(faceids.begin(), faceids.end());
        halfedges[m2a(a[0],a[1])] = true;
        halfedges[m2a(a[1],a[2])] = true;
        halfedges[m2a(a[2],a[0])] = true;
        trFaces.erase(it1);
        success = true;
        break;
      }
    }
  }
  return success;
}


template <class HDS>
bool ConstructShell<HDS>::is_connected(int* tr, bool* halfedges)
{
  if ( (halfedges[m2a(tr[1],tr[0])] == true) ||
       (halfedges[m2a(tr[2],tr[1])] == true) ||
       (halfedges[m2a(tr[0],tr[2])] == true) )
    return true;
  else 
    return false;
}

template <class HDS>
void ConstructShell<HDS>::add_one_face(CGAL::Polyhedron_incremental_builder_3<HDS>& B, int i0, int i1, int i2, int faceID)
{
  std::vector< std::size_t> faceids(3);        
  faceids[0] = i0;
  faceids[1] = i1;
  faceids[2] = i2;
  if (B.test_facet(faceids.begin(), faceids.end()))
      B.add_facet(faceids.begin(), faceids.end());
  else
  { //-- reverse the face and test if it would be possible to insert it
    isValid = false;
    faceids[0] = i0;
    faceids[1] = i2;
    faceids[2] = i1;
    if (B.test_facet(faceids.begin(), faceids.end()))
    {
      //std::cout << "*** Reversed orientation of the face" << std::endl;
      sh->add_error(307, faceID, ""); 
 }
    else
      sh->add_error(304, faceID, ""); //-- >2 surfaces incident to an edge: non-manifold
  }
  return ;
} 


// CgalPolyhedron* validate_triangulated_shell_cs(TrShell& tshell, int shellID, cbf cb, double TOL_PLANARITY_normals)
// {
//   bool isValid = true;
//   CgalPolyhedron *P = new CgalPolyhedron;

// //-- 1. Planarity of faces
//   if (isValid == true) 
//   {
//     (*cb)(0, -1, -1, "-----Planarity");
//     if (check_planarity_normals(tshell.faces, tshell.lsPts, shellID, cb, TOL_PLANARITY_normals) == false)
//       isValid = false;
//     else
//       (*cb)(0, -1, -1, "\tyes");
//   }
    
// //-- 2. Combinatorial consistency
//   if (isValid == true)
//   {
//     (*cb)(0, -1, -1, "-----Combinatorial consistency");

//     //-- construct the CgalPolyhedron incrementally
//     ConstructShell<HalfedgeDS> s(&(tshell.faces), &(tshell.lsPts), shellID, false, cb);
//     P->delegate(s);
//     isValid = s.isValid;
//     if (isValid == true)
//     {
//       if (P->is_valid() == true) //-- combinatorially valid that is
//       {
//         P->normalize_border();
//         if (P->keep_largest_connected_components(1) > 0)
//           {
//             (*cb)(305, shellID, -1, "");
//             isValid = false;
//           }
//       }
//       else 
//       {
//         (*cb)(300, shellID, -1, "Something weird went wrong during construction of the shell, not sure what...");
//         isValid =  false;
//       }
//     }
//   }

// //-- 3. Geometrical consistency (aka intersection tests between faces)
//   if (isValid == true)
//   {
//     (*cb)(0, -1, -1, "\tyes");
//     (*cb)(0, -1, -1, "-----Geometrical consistency");
//     isValid = is_polyhedron_geometrically_consistent(P, shellID, cb);
//   }
//   if (isValid)
//   {
//   (*cb)(0, -1, -1, "\tyes\n");
//   }
// //-- Return CgalPolyhedron if valid, NULL otherwise
//   if (isValid == false)
//   {
//     delete P;
//     P = NULL;
//   }
//   return P;
// }



//revised version of check_global_orientation_normals
//should be careful!! still have contradict cases but low probability
bool check_global_orientation_normals_rev( CgalPolyhedron* p, bool bOuter)
{
	/*calculation the AABB tree for select an exterior point outside the bounding box
	and calculate intersections for the ray. selected the closest intersection point and comparing the normal*/
	
	//definitions
	typedef K::Ray_3 Ray;
	typedef K::Triangle_3 Triangle;
	typedef list<Triangle>::iterator Iterator;
	typedef CGAL::AABB_polyhedron_triangle_primitive<K, CgalPolyhedron> Primitive;
	typedef CGAL::AABB_traits<K, Primitive> Traits;
	typedef CGAL::AABB_tree<Traits> AABBTree;

	//build the aabbtree for polyhedron
	AABBTree myTree(p->facets_begin(), p->facets_end());
	if (myTree.empty())
	{
		cout<<"Unable to construct AABB tree"<<endl;;
		return false;
	}
	myTree.accelerate_distance_queries();

	//calculate the exterior point (corner of bbox and shift a bit)
	AABBTree::Bounding_box bbox = myTree.bbox();

	//find the first valid result from the triangle
	float fShift = 100.0;// shift the coordinate of the projected points
	CgalPolyhedron::Facet_iterator fItr = p->facets_begin();
	while (fItr != p->facets_end())
	{
		CgalPolyhedron::Facet_handle fHandle = fItr;
		CgalPolyhedron::Halfedge_handle h = fHandle->halfedge();

		//project the face to one of the axis aligned plane
		Vector vecNormal = CGAL::cross_product(h->next()->vertex()->point() - h->vertex()->point(),
			h->next()->next()->vertex()->point() - h->next()->vertex()->point());
		
		//select the projection plane
		K::FT a = abs(vecNormal * Vector(1.0, 0.0, 0.0));//yz
		K::FT b = abs(vecNormal * Vector(0.0, 1.0, 0.0));//xz
		K::FT c = abs(vecNormal * Vector(0.0, 0.0, 1.0));//xy

		K::FT m = max(max(a, b), c);

		//construct the ray based on the projection
		Ray queryRay;
		Point3 stPt;
		if (CGAL::compare(a, m) == CGAL::EQUAL)
		{
			//yz
			stPt = Point3(bbox.xmax() + fShift, 
				(h->vertex()->point().y() + h->next()->vertex()->point().y() + h->next()->next()->vertex()->point().y())/3, 
				(h->vertex()->point().z() + h->next()->vertex()->point().z() + h->next()->next()->vertex()->point().z())/3);
			Vector vecInwards (-1.0, 0.0, 0.0);
			queryRay = Ray(stPt, vecInwards);
		}
		else if (CGAL::compare(b, m) == CGAL::EQUAL)
		{
			//xz
			stPt = Point3((h->vertex()->point().x() + h->next()->vertex()->point().x() + h->next()->next()->vertex()->point().x())/3,
				bbox.ymax() + fShift, 
				(h->vertex()->point().z() + h->next()->vertex()->point().z() + h->next()->next()->vertex()->point().z())/3);
			Vector vecInwards (0.0, -1.0, 0.0);
			queryRay = Ray(stPt, vecInwards);
		}
		else
		{
			//xy
			stPt = Point3((h->vertex()->point().x() + h->next()->vertex()->point().x() + h->next()->next()->vertex()->point().x())/3,
				(h->vertex()->point().y() + h->next()->vertex()->point().y() + h->next()->next()->vertex()->point().y())/3,
				bbox.zmax() + fShift);
			Vector vecInwards (0.0, 0.0, -1.0);
			queryRay = Ray(stPt, vecInwards);
		}

		//shoot the ray and extract the closed intersection (point and its face)
		list<AABBTree::Object_and_primitive_id> intersections;
		myTree.all_intersections(queryRay, std::back_inserter(intersections));
		//
    K::FT fDistance;
		AABBTree::Object_and_primitive_id Hit;
		//
		list<AABBTree::Object_and_primitive_id>::iterator listItr = intersections.begin();
		CGAL::Object tmpObj = listItr->first;
		Point3 tmpPt;
		if (CGAL::assign(tmpPt, tmpObj))
		{
			fDistance = (tmpPt - stPt).squared_length();
			Hit = *listItr;
		}
		else
		{
			cout<<"Unbelievable!!"<<endl;
			break;
		}
		//
		while(listItr != intersections.end())
		{
			AABBTree::Object_and_primitive_id op = *listItr;
			CGAL::Object obj = op.first;
			Point3 pt;
			if (CGAL::assign(pt, obj))
			{
				//calculate the distance
        K::FT dist = (pt - stPt).squared_length();
				if (fDistance > dist)
				{
					fDistance = dist;
					Hit = *listItr;
					//cout<<fDistance<<endl;
				}
			}
			else
			{
				cout<<"Unbelievable!!"<<endl;
				break;
			}
			++listItr;
		}
		
		//make sure the point is not one of the vertices
		bool bBadLuck = false;
		CgalPolyhedron::Vertex_iterator vItr = p->vertices_begin();
		for (; vItr != p->vertices_end(); vItr++)
		{
			Point3 pt;
			CGAL::assign(pt, Hit.first);
			if (pt == vItr->point())
			{
				bBadLuck = true;
				break;
			}
		}

		//get the answer by comparing the normals
		if (!bBadLuck)
		{
			//calculate the normal of the face
			CgalPolyhedron::Halfedge_handle h = Hit.second->halfedge();
			//cout<<h->vertex()->point()<<" "<<h->next()->vertex()->point()<<" "<<h->next()->next()->vertex()->point()<<endl;
			Vector faceNormal = CGAL::cross_product(h->next()->vertex()->point() - h->vertex()->point(),
				h->next()->next()->vertex()->point() - h->next()->vertex()->point());
			//compare
			K::FT result = faceNormal * queryRay.to_vector();
			if (CGAL::compare(result, 0.0) == CGAL::LARGER)
			{
				return !bOuter;
			}
			else if(CGAL::compare(result, 0.0) == CGAL::SMALLER)
				return bOuter;
		}
		++fItr;
	}
	return false;
}

//using local method proposed by Sjors Donkers
//Requirments: The input polyhedron should be valid without intersections or degeneracies.
bool check_global_orientation_normals_rev2( CgalPolyhedron* p, bool bOuter)
{
	//-- get a 'convex corner' along x
	CgalPolyhedron::Vertex_iterator vIt;
	vIt = p->vertices_begin();
	CgalPolyhedron::Vertex_handle cc = vIt;
	vIt++;

	for ( ; vIt != p->vertices_end(); vIt++)
	{
		if (vIt->point().x() > cc->point().x())
			cc = vIt;
	}
	//-- crop the convex corner using plane yz (x = cc->point().x() - TOL) and get the cropped 2D polygon 
	K::FT TOL(1e-8);
	CgalPolyhedron::Halfedge_around_vertex_circulator hCirc = cc->vertex_begin();//clockwise
	int iNum = CGAL::circulator_size(hCirc);
	Polygon lPgn;
	for (int i = 0; i < iNum; i++)
	{
		//calculate the intersection
		Vector vecR(hCirc->vertex()->point(), hCirc->opposite()->vertex()->point());
		vecR = vecR / sqrt(vecR.squared_length());
		if (CGAL::compare(vecR.x(), 0.0) != CGAL::EQUAL)
		{
			K::FT para = -TOL/vecR.x(); // (cc->point().x() - TOL - cc->point().x()) / vecR.x()
			K::FT y_2d = para * vecR.y() + cc->point().y();
			K::FT z_2d = para * vecR.z() + cc->point().z();
			lPgn.push_back(Polygon::Point_2(y_2d, z_2d));
		}
		else
		{
			//use the parallel edge (convex edge) to decide the orientation
			//The same as 2D method
			CGAL::Orientation orient = orientation( hCirc->vertex()->point(),
				hCirc->next()->vertex()->point(),
				hCirc->next()->next()->vertex()->point(),
				hCirc->opposite()->next()->vertex()->point() );
			if (orient == CGAL::NEGATIVE)
			{
				return bOuter;
			}
			else if (orient == CGAL::POSITIVE)
			{
				return !bOuter;
			}
			else
			{
				//coplanar facets
				Vector vecX (1.0, 0.0, 0.0);
				CgalPolyhedron::Plane_3 plane (hCirc->vertex()->point(),
					hCirc->next()->vertex()->point(),
					hCirc->next()->next()->vertex()->point());
				if (!plane.is_degenerate())
				{
					if (plane.orthogonal_vector()*vecX > 0)
					{
						return bOuter;
					}
					else
					{
						return !bOuter;
					}
				}
			}
		}
		hCirc++;
	}

	//-- check the orientation 
	if (lPgn.is_simple())
	{
		if(lPgn.orientation() == CGAL::CLOCKWISE)
		{
			//equal to the order of circulator
			return bOuter;
		}
		else if(lPgn.orientation() == CGAL::COUNTERCLOCKWISE)
		{
			//unequal to the order of circulator
			return !bOuter;
		}
		else//collinear
			return check_global_orientation_normals_rev(p, bOuter);
	}
	else
	{
		//do ray shooting
		return check_global_orientation_normals_rev(p, bOuter);
	}
}

//problematic
bool check_global_orientation_normals( CgalPolyhedron* p, bool bOuter)
{
	//-- get a 'convex corner', sorting order is x-y-z
	CgalPolyhedron::Vertex_iterator vIt;
	vIt = p->vertices_begin();
	CgalPolyhedron::Vertex_handle cc = vIt;
	vIt++;

	for ( ; vIt != p->vertices_end(); vIt++)
	{
		if (vIt->point().x() > cc->point().x())
			cc = vIt;
		else if (vIt->point().x() == cc->point().x())
		{
			if (vIt->point().y() > cc->point().y())
				cc = vIt;
			else if (vIt->point().y() == cc->point().y())
			{
				if (vIt->point().z() > cc->point().z())
					cc = vIt;
			}
		}
	}
	//  cout << "CONVEX CORNER IS: " << cc->point() << endl;
	CgalPolyhedron::Halfedge_handle curhe = cc->halfedge();
	CgalPolyhedron::Halfedge_handle otherhe;
	otherhe = curhe->opposite()->next();
	CGAL::Orientation orient = orientation( curhe->vertex()->point(),
	                                        curhe->next()->vertex()->point(),
	                                        curhe->next()->next()->vertex()->point(),
	                                        otherhe->vertex()->point() );
	while (orient == CGAL::COPLANAR)
	{
	   otherhe = otherhe->next()->opposite()->next();
	   if (otherhe->next() == curhe->next()->opposite())
	   {
		    //finished the traverse
		   break;
	   }
	   orient = orientation( curhe->vertex()->point(),
	                         curhe->next()->vertex()->point(),
	                         curhe->next()->next()->vertex()->point(),
	                         otherhe->vertex()->point() );
  }
	if ( ((bOuter == true) && (orient != CGAL::CLOCKWISE)) || ((bOuter == false) && (orient != CGAL::COUNTERCLOCKWISE)) ) 
		  return false;
	else
		  return true;

	//revised by John to check whether the normals of the star of the vertex point to the "peak" direction
	/*bool bOrient = true;
	CgalPolyhedron::Halfedge_around_vertex_circulator hcir = curhe->vertex()->vertex_begin();
	int iNum = CGAL::circulator_size(hcir);
	Vector x_direction(1.0, 1.0, 1.0);

	for(int i = 0 ; i < iNum; ++i)
	{
	CgalPolyhedron::Plane_3 plane = CgalPolyhedron::Plane_3(hcir->vertex()->point(),
	hcir->next()->vertex()->point(),
	hcir->next()->next()->vertex()->point());

	cout<<x_direction * plane.orthogonal_vector()<<endl;
	cout<<hcir->vertex()->point()<<", "<<
	hcir->next()->vertex()->point()<<", "<<
	hcir->next()->next()->vertex()->point()<<endl;

	if (CGAL::compare(x_direction * plane.orthogonal_vector(), 0.0) == CGAL::SMALLER)
	{
	bOrient  = false;
	break;
	}

	++hcir;
	}

	if ( ((bOuter == true) && (bOrient)) || ((bOuter == false) && (!bOrient)) ) 
	return true;
	else
	return false;*/
}


CgalPolyhedron* construct_CgalPolyhedron_batch(const vector< vector<int*> >&lsTr, const vector<Point3>& lsPts)
{
  //-- construct the 2-manifold, using the "batch" way
  stringstream offrep (stringstream::in | stringstream::out);
  vector< vector<int*> >::const_iterator it = lsTr.begin();
  size_t noFaces = 0;
  for ( ; it != lsTr.end(); it++)
    noFaces += it->size();
  offrep << "OFF" << endl << lsPts.size() << " " << noFaces << " 0" << endl;

  vector<Point3>::const_iterator itPt = lsPts.begin();
  for ( ; itPt != lsPts.end(); itPt++)
    offrep << *itPt << endl;

  for (it = lsTr.begin(); it != lsTr.end(); it++)
  {
    vector<int*>::const_iterator it2 = it->begin();
    for ( ; it2 != it->end(); it2++)
    {
      int* tmp = *it2;
      offrep << "3 " << tmp[0] << " " << tmp[1] << " " << tmp[2] << endl;
    }
  }
  CgalPolyhedron* P = new CgalPolyhedron();
  offrep >> *P;
  return P;
}


