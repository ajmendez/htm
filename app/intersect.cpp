//#     Filename:       intersect.cpp
//#
//#     Intersect a domain with the index, returning the full and partial nodes
//#
//#
//#     Author:         Peter Z. Kunszt, Gyorgy Fekete
//#
//#     Date:           October 15, 1999
//#                     November 11, 2002 Gyorgy Fekete
//#                     Fixed  a few bugs.
//#
//#
//# This program is free software; you can redistribute it and/or
//# modify it under the terms of the GNU General Public License
//# as published by the Free Software Foundation; either version 2
//# of the License, or (at your option) any later version.
//#
//# This program is distributed in the hope that it will be useful,
//# but WITHOUT ANY WARRANTY; without even the implied warranty of
//# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//# GNU General Public License for more details.
//#
//# You should have received a copy of the GNU General Public License
//# along with this program; if not, write to the Free Software
//# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//#
//#

#include <iostream>

#define _INC_IOSTREAM


#include <SpatialGeneral.h>
#include <SpatialIndex.h>
#include <SpatialConstraint.h>
#include <RangeConvex.h>
#include <SpatialDomain.h>
#include <sqlInterface.h>
#include <HtmRange.h>
#include <HtmRangeIterator.h>


#include <fstream>
#include <time.h>


bool visualize = false;
bool verbose = false;		// verbosity flag
bool compress = false;         // whether or not to compress ranges;
bool varlength = false;       // output variable length HTMids
bool expand = false;       // output all hids (could be long list!)
bool symbolic = false;

int arg = 2;			// number of required arguments

char *infile;			// infile name
int depth,savedepth=5;	// depth and stored depth
int maximum_nrGaps=100;
int olevel = -1;

char htmidfilename[1024];
char domainfilename[1024];

size_t i,j;

void
usage(char *name) {

  std::cerr << "usage: " << std::endl
       << name << " [-save savelevel] [-verbose] [-olevel olevel] level domainfile" << std::endl;
  std::cerr << " [-save savelevel]   : store up to this depth (default 2)" << std::endl
       << " [-verbose]          : verbose" << std::endl
       << " [-olevel out_level] : output HTMID level (must be >= level)" << std::endl
       << " [-symbolic]         : output HTMID as a symbolic name, must be used with -varlength" << std::endl
       << " [-expand]           : output all HTMIDs in the range (conflicts with -varlength)" << std::endl
       << " [-varlength]        : output natural HTMID (conflicts with -olevel)" << std::endl
       << " [-nranges nrages]   : keep number of ranges below this number" << std::endl
    //       << " [-visualize domainfile hidfile] : creates two files to be used with htmviz openGL application" << std::endl
       << " level               : Maximal spatialindex depth " << std::endl
       << " domainfile          : filename of domain " << std::endl
       << std::endl;
  exit(1);
}

int init_all(int argc, char *argv[])
{

  int args = 1;			// start to parse argument no 1
  argc--;

  while(argc > 0) {
    if(strcmp(argv[args],"-verbose")==0) {
      verbose = true;
    } else if(strcmp(argv[args],"-visualize")==0) {
      visualize = true;
      if (argc > 2){
	strcpy(domainfilename, argv[++args]); argc--;
	strcpy(htmidfilename, argv[++args]); argc--;
      } else {
	usage(argv[0]);
      }
    } else if (strcmp(argv[args],"-expand")==0) {
      expand = true;
    } else if (strcmp(argv[args],"-symbolic")==0) {
      symbolic = true;
    } else if (strcmp(argv[args],"-varlength")==0) {
      varlength = true;
    } else if(strcmp(argv[args],"-save")==0) {
      savedepth = atoi(argv[++args]); argc--;
      if(savedepth < 1 || savedepth > 8) {
	std::cerr << "savedepth should be between 1 and 8, is " << savedepth << std::endl;
	return -1;
      }
    } else if(strcmp(argv[args],"-olevel")==0) {
      olevel = atoi(argv[++args]); argc--;
    } else if(strcmp(argv[args],"-nranges")==0) {
      maximum_nrGaps = atoi(argv[++args]); argc--;
      compress = true;
    } else {
      if( *argv[args] == '-' ) usage(argv[0]);
      switch(arg) {
      case 2:
	depth = atoi(argv[args]);
	if(depth < 1 || depth > HTMMAXDEPTH) {
	  std::cerr << "depth should be between 1 and " << HTMMAXDEPTH << ", is " << depth << std::endl;
	  return -1;
	}
	break;
      case 1:
	infile = argv[args];
	break;
      default:
	usage(argv[0]);
      }
      arg--;
    }
    argc--;
    args++;
  } // while (argc > 0)

  if (olevel < depth)
    olevel = depth;

//   if (symbolic && !varlength)
//     usage(argv[0]);

  std::cerr << "Depth = " << depth << ", output level = " << olevel << std::endl;
  return 0;
}
// intopends60.lib 


int main(int argc, char *argv[]) {

/*******************************************************
/
/ Initialization
/
 ******************************************************/
  Key gapsize;
  int rstat = init_all(argc, argv);
  if (rstat != 0){
    return rstat;
  }

  if(arg > 0)
    usage(argv[0]);

  std::ifstream in(infile);
  if(!in) {
    std::cout << "cannot open file " << infile << std::endl;
    return -1;
  }

  try
    {

      // construct index with given depth and savedepth

      htmInterface htm(depth,savedepth);  // generate htm interface
      const SpatialIndex &index = htm.index();
      HtmRange htmRange;

      // Read in domain and echo it to screen

      SpatialDomain domain;   // initialize empty domain
      in >> domain;	      // read domainfile
      domain.setOlevel(olevel);
      if(verbose)
	      std::cout << domain;

      if (visualize){
	// A file for hids, and a file for the domain
	std::ofstream outfile(domainfilename, std::ios::out);
	if (outfile){
	  outfile << domain;
	  outfile.close();
	}
      }
      // Write domain to a file
/*******************************************************
/
/ Intersection
/
 ******************************************************/
      // do intersection and time it
      time_t t0 = clock();
      j = 1; // Why would you want it more? Timing, perhaps...

      // cerr << "Normal intersect starts here" << std::endl;
      htmRange.purge();
      while(j--) {
	domain.intersect(&index, &htmRange, varlength);	  // intersect with range
      }
      time_t t1 = clock();
      if(verbose) {
	printf("%d intersections done at a rate of %f intersections/sec\n",
	       1, ((float)1)/(((double)(t1-t0)/(double)CLOCKS_PER_SEC)));
      }
      
/*******************************************************
/
/ Print result
/
 ******************************************************/
      if (varlength){
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   only print lows (should be same as highs)
	htmRange.print(HtmRange::LOWS, std::cout, symbolic);
	if (visualize) { 	// print hids to file too
	  std::ofstream outfile(htmidfilename, std::ios::out);
	  if (outfile){
	    htmRange.print(HtmRange::LOWS, outfile, symbolic);
	    outfile.close();
	  }
	}
      } else {
	htmRange.defrag();
	if (compress){
	  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   find best gap, and merge (defrag) ranges
	  gapsize = htmRange.bestgap(maximum_nrGaps);
	  htmRange.defrag(gapsize);
	  std::cerr << "Combining ranges with gaps <= " << gapsize;
	  std::cerr << " with no more than " << maximum_nrGaps << std::endl;
	}
	if (expand){
	  HtmRangeIterator iter(&htmRange);
	  if (!symbolic){
	    while (iter.hasNext()) {
	      std::cout << iter.next() << std::endl;
	    }
	  } else {
	    char buffer[80];
	    while (iter.hasNext()) {
	      std::cout << iter.nextSymbolic(buffer) << std::endl;
	    }
	  }
	} else {
	  htmRange.setSymbolic(symbolic);
	  std::cout << htmRange;
	}
      }
    }// Try
  catch (SpatialException &x)
    {
      printf("%s\n",x.what());
    }
  return 0;
}
