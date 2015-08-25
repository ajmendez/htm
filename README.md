# htm
Hierarchical Triangular Mesh

This is a copy of the SDSS HTM implementation from [1] developed by O'Mullane, Kunszt, Szalay


# Original Readme

       	       	       	       	       	       	       	       	       	       |
Configurtion notes:
There is a Makefile.generic in this directory. Makefile is made by the 
configure shell script. DO NOT EDIT Makefile, edit Makefile.generic instead.
The plan is to have all system related things defined as (see Makefile.generic
for an example) stuff = @@stuff## and have a stream editor change them 
everywhere. Only u*x (unix, linux, irix, darwin, bsd, solaris...) benefits,
Windows flavors have their own project files, and one day, nmakefiles


There are several changes in this release. The bitlist is gone. Partial and
full lists are gone. They have been merged into a data datype called HtmRange.

This single data type can represent many ways of looking at large (or small)
numbers of HTMIDs.

The (inaptly named) application "intersect" reads domain descripion files
and spits out hids (HTM-ID's) to the standard output. Depending on what
the user wants the number of hids can be potentially very large. Consider
a case where the DOMAIN consists of a large area, and the application
asks to produce all hids at level 20! For a concrete example, consider a
small disk around the "north pole" described by the DOMAIN description
file: (testTiny in this directory)
    ----------------------
    #DOMAIN
    1
    #CONVEX
    1
    0.0 0.0 1.0 0.9999
    ----------------------
The number of level 20 triangles or, as we now call them, trixels (triangle
shaped pixels) is on the order of about a billion (10^9). It may be
unfeasible to put all those numbers (hids) into the output stream, so
HtmRanges allow us to exploit the numeric coherence implicit in large sets
of hids. Although in general, it is not necessarily true that two consecutive
hids represent adjacent trixels, it is true, that the set of all hids derived
from a single parent triangle form a single connected component. In plain
words, if you sort them there are no gaps. Therefore, it is convenient
to represent these large chunks of hids as a range, or more precisely,
an ordered pair: (low_hid, high_hid). Any arbitrary DOMAIN is therefore
represented as a collection (list, vector, array, etc...) of (lo, hi)
pairs.

The best way to play with the HTM interface is by playing with the
intersect application.


Consider the DOMAIN description
    ---------------testTiny ------------------------
    #DOMAIN
    1
    #CONVEX
    1
    0.0 0.0 1.0 0.9999
    ---------------END: testTiny--------------------

intersect produces the ranges

    % intersect 20 testTiny
    13469017440256 13469285875711
    14568529068032 14568797503487
    15668040695808 15668309131263
    16767552323584 16767820759039

Consider the DOMAIN description
If you desire to see the largest triangles represented by an hid that
is still contained in the DOMAIN, use an option that tells the application
to produce variable length hids instead of ranges.

    % intersect -varlength 20 testTiny
    50176
    54272
    58368
    62464
    
--------- further examples -

The file testInputIntersect contains the following DOMAIN description:

    ------------------- testInputIntersect -----------------
    #DOMAIN
    1
    #CONVEX
    3
    0.9 0.007107 0.05 0.780775301220802
    0.5 0.5 0.707107 0.63480775301220802
    0.707107 -0.5 0.3 0.8480775301220802
    ------------------- END: testInputIntersect -------------

Run the intersect program produces 141 ranges. The first three
and the last three are shown here:


    % intersect  20 testInputIntersect
    13211319402496 13213466886143
    13213802430464 13213803479039
    13214003757056 13214305746943
    ...
    14214513033216 14214529810431
    14215536443392 14215670661119
    14215771324416 14215788101631


The variable length option would produce 207 hids. The first and last are
respectively:

    3089
    14170322436984

Representing trixels 'N0100000' and 'N032031021002100031320'. Run intersect
without arguments to get usage info:

    -----------------------------------------------------------------------------------
    usage: 
    intersect [-save savelevel] [-verbose] [-olevel olevel] level domainfile
    [-save savelevel]   : store up to this depth (default 2)
    [-verbose]          : verbose
    [-olevel out_level] : output HTMID level (must be >= level)
    [-symbolic]         : output HTMID as a symbolic name, must be used with -varlength
    [-varlength]        : output natural HTMID (conflicts with -olevel)
    [-nranges nrages]   : keep number of ranges below this number
    level               : Maximal spatialindex depth 
    domainfile          : filename of domain 
    -----------------------------------------------------------------------------------



		    Comments about this READ_ME? Please send me mail
		    mailto:gfekete@pha.jhu.edu

Some stats:
==========
At level 20
the largets HID  is  N333333333333333333333 = 17592186044415
The smallest HID is  S000000000000000000000 =  8796093022208
The difference is
   17592186044415 
  - 8796093022208 =  ....................   =  8796093022207

17,592,186,044,415
 8,796,093,022,207

about 17 and a half trillion.
difference is about 8 and three quarters trillion.

At level 12
The largest HID is   N3333333333333 = 268435455
The smallest HID is  S0000000000000 = 134217728   268435455-134217728
The difference is                   = 134217727
                                       16777219 
268,435,455
134,217,728

about 268  million.
difference is about 134 million.

(16777219 16777230 369098836)
16777219 




[1] http://www.skyserver.org/htm/implementation.aspx
