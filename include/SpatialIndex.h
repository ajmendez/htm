#ifndef _SpatialIndex_h
#define _SpatialIndex_h

//#     Filename:       SpatialIndex.h
//#
//#     SpatialIndex is the class for the the sky indexing routines.
//#
//#     Author:         Peter Z. Kunszt, based on A. Szalay s code
//#
//#     Date:           October 15, 1998
//#
//#		Copyright (C) 2000  Peter Z. Kunszt, Alex S. Szalay, Aniruddha R. Thakar
//#                     The Johns Hopkins University
//#     Modification History:
//#
//#     Oct 18, 2001 : Dennis C. Dinge -- Replaced ValVec with std::vector
//#	    Sept 9, 2002 : Gyorgy Fekete -- added setMaxlevel()
//#

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <SpatialGeneral.h>
#include <SpatialVector.h>
#include <SpatialEdge.h>
#include <SpatialException.h>
  // begin add dcd
#include <vector>
#include <string>
#include <algorithm>


//########################################################################
//#
//# Spatial Index class 
//#


/**
   The Spatial Index is a quad tree of spherical triangles. The tree
   is built in the following way: Start out with 8 triangles on the
   sphere using the 3 main circles to determine them. Then, every
   triangle can be decomposed into 4 new triangles by drawing main
   circles between midpoints of its edges:

<pre>

.                            /\
.                           /  \
.                          /____\
.                         /\    /\
.                        /  \  /  \
.                       /____\/____\

</pre> 
   This is how the quad tree is built up to a certain level by
   decomposing every triangle again and again.
*/



class LINKAGE SpatialIndex {
public:
    size_t getMaxlevel(){return maxlevel_;};		// the depth of the Layer
    size_t getBildLevel(){return buildlevel_;};	// the depth of the Layer storedbuildlevel_;

  /** Constructor.
      Give the level of the index and optionally the level to build -
      i.e. the depth to keep in memory.  if maxlevel - buildlevel > 0
      , that many levels are generated on the fly each time the index
      is called. */
  SpatialIndex() {};
  SpatialIndex(size_t maxlevel, size_t buildlevel =5);

  /// NodeName conversion to integer ID
  static uint64 idByName(const char *);

  /** int32 conversion to a string (name of database).
      WARNING: if name is already allocated, a size of at least 17 is
      required.  The conversion is done by directly calculating the
      name from a number.  To calculate the name of a certain level,
      the mechanism is that the name is given by (#of nodes in that
      level) + (id of node).  So for example, for the first level,
      there are 8 nodes, and we get the names from numbers 8 through
      15 giving S0,S1,S2,S3,N0,N1,N2,N3.  The order is always
      ascending starting from S0000.. to N3333...  */
  static char * nameById(uint64 ID, char * name = 0);

  /** Return leaf number in bitlist for a certain ID.  Since the ID
      here means the number computed from the name, this is simply
      returning ID -leafCount().  Bitlists only work until level 14.*/
  uint32 leafNumberById(uint64 ID) const;

  /** Return leaf id for a certain bitlist index. 
      Same as the function above */
  uint64 idByLeafNumber(uint32 n) const ;

  /** return name for a certain leaf index (to be used for name lookup
      from a bitlist).  This function is simply shorthand for
      nameById(n + leafCount()).  */
  char * nameByLeafNumber(uint32 n, char * name = 0) const;

  /** find the vector to the centroid of a triangle represented by 
	  the ID */
  void pointById(SpatialVector & vector, uint64 ID) const;

  /** find a node by giving a vector. 
      The ID of the node is returned. */
  uint64 idByPoint(SpatialVector & vector) const;

  /// find a node by giving a ra,dec in degrees.
  uint64 idByPoint(const float64 & ra, const float64 & dec) const;

  /// find a node by giving a vector. 
  /**@return The ID of the node is returned. */
  char* nameByPoint(SpatialVector & vector, char* s=NULL) const;

  /// find a node by giving a ra,dec in degrees.
  char* nameByPoint(const float64 & ra, const float64 & dec, 
		    char* s=NULL) const;

  /// return number of leaf nodes
  uint64 leafCount() const;

  /// return number of vertices
  size_t nVertices() const;

  /// The area in steradians for a given index ID
  float64 area(uint64 ID) const;

  /// The area in steradians for a given spatial triangle
  float64 area(const SpatialVector & v1, 
	       const SpatialVector & v2, 
	       const SpatialVector & v3) const;

  /// return the actual vertex vectors
  void nodeVertex(const uint64 id, 
     		  SpatialVector & v1, 
		  SpatialVector & v2, 
		  SpatialVector & v3) const; 

  /// return index of vertices for a node
  void nodeVertex(const size_t idx, 
		  size_t & v1, size_t & v2, size_t & v3) const; 

  /// print all vertices to output stream
  void showVertices(std::ostream & out) const;

  /// set the maximum depth of the layer
  void setMaxlevel(int level);

private:

  // STRUCTURES

  struct Layer {
    size_t 	level_;			// layer level
    size_t 	nVert_;			// number of vertices in this layer
    size_t 	nNode_;			// number of nodes
    size_t 	nEdge_;			// number of edges
    uint64 	firstIndex_;	// index of first node of this layer
    size_t 	firstVertex_;	// index of first vertex of this layer
  };

  struct QuadNode {
    uint64	index_;			// its own index
    size_t	v_[3];			// The three vertex vector indices
    size_t	w_[3];			// The three middlepoint vector indices
    uint64	childID_[4];	// ids of children
    uint64	parent_;		// id of the parent node (needed for sorting)
    uint64	id_;			// numeric id -> name
  };

  // add quadnode vector
  typedef std::vector<QuadNode> ValueVectorQuad;

  // FUNCTIONS

  // insert a new node_[] into the list. The vertex indices are given by
  // v1,v2,v3 and the id of the node is set.
  uint64 newNode(size_t v1, size_t v2,size_t v3,uint64 id,uint64 parent);

  // make new nodes in a new layer.
  void makeNewLayer(size_t oldlayer);

  // return the total number of nodes and vertices
  void vMax(size_t *nodes, size_t *vertices);

  // sort the index so that the leaf nodes are at the beginning
  void sortIndex();

  // Test whether a vector v is inside a triangle v0,v1,v2. Input
  // triangle has to be sorted in a counter-clockwise direction.
  bool isInside(const SpatialVector & v, const SpatialVector & v0,
		const SpatialVector & v1, const SpatialVector & v2) const;

  // VARIABLES

  size_t 		maxlevel_;		// the depth of the Layer
  size_t 	    buildlevel_;	// the depth of the Layer stored
  uint64		leaves_;		// number of leaf nodes
  uint64		storedleaves_;	// number of stored leaf nodes
  ValueVectorQuad   nodes_;		// the array of nodes 
  std::vector<Layer> layers_;	// array of layers

  typedef std::vector<SpatialVector> ValueVectorSpvec;
  ValueVectorSpvec vertices_;
  uint64 		index_;			// the current index_ of vertices

  friend class SpatialEdge;
  friend class SpatialConvex;
  friend class RangeConvex;
  friend class SpatialDomain;
  friend class htmInterface;
};


#include "SpatialIndex.hxx"
#endif
