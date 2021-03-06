This file is protected by Copyright. Please refer to the COPYRIGHT file 
distributed with this source distribution.

This file is part of GNUHAWK.

GNUHAWK is free software: you can redistribute it and/or modify is under the 
terms of the GNU General Public License as published by the Free Software 
Foundation, either version 3 of the License, or (at your option) any later 
version.

GNUHAWK is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with 
this program.  If not, see http://www.gnu.org/licenses/.

Table of Contents:

Adding New QA Components to Build Environment
Running QA Tests


############################################
Adding New QA Components to Build Environment
############################################

1) Generate component using REDHAWK IDE and GNUHAWK code generator

2) cp component directory under gnuhawk/qa/components

3) from qa directory 
   ./bld/fixcomp  <component directory name >

4) add component to build environment
   vi bld/cdirs

5) Rebuild QA components:
   ./reconf; ./configure; make
  
   or 

   cd components/<component name>/cpp
   ./reconf;./configure; make

   or 


   -- to build with gnuhawk sdr dependency
   cd components/<component name>/cpp
   ./configure --enable-deps=sdr
   


########################################
Running QA Tests
########################################
Usage: ./runtests.py <qa filename matching string> [options]

Options:
  --prefix=PREFIX      only run tests with this prefix
  --debug=DEBUG        add debug param to component instantiation

To run all qa tests contained in the tests subdirectory:
    "./runtests.py"
If qa filename is not specified, the default is "qa_*.py"

To run a subset of tests or an individual test:
    ex. "./runtests.py qa_int_to_float.py --prefix=test_002"  or "./runtests.py qa_int_to*.py"

If prefix is not specified, the default is "test"

The debug level sets the log4cxx logging level for the instantiated component by passing in the execparam
DEBUG_LEVEL.  Possible values: 0 for FATAL, 1 for ERROR, 2 for WARN, 3 for
INFO, 4 for DEBUG, and 5 for TRACE/ALL.

The structure of gr and gr_unittest.py is set up such that this version of 
gr_unittest.py can be imported in place of the gnuradio version of gr_unittest.py.

Helper modules and classes
-------------------------
1. _gnuComponentWrappers.py: 
  - contains methods that override the gnuradio block classes returning Redhawk 
    sandbox components.
  NOTE: This file can be regenerated by running 
        gnuhawk/qa/gnuradio/gr/genGnuComponentWrappers.py which will go out to 
        the local SDRROOT and insert methods for any components matching the 
        RH_GR_PREFIX currently set to "rh_gr". However, the order of arguments 
        can matter in some qa tests, so the generated method will probably have 
        to be hand modified.  vlen for example is sometimes the first argument in 
        a gnuradio block constructor and sometimes it is the last. It is
        suggested to just copy and paste a current method definition and edit
        that to match the constructor found in the original gnuradio block.

2. gnuradioStubs.py: 
  - contains  overridden python classes used in the qa tests
    ex. head, vector_to_stream, stream_to_vector

3. gnuhawk/qa/components: 
  - contains helper Redhawk components used in the execution  of qa tests
   ex. gr_head_complex, gr_head_float, etc.

4. gnuhawk/qa/swig_modules: 
  - contains empty modules to override expected swig modules referenced in original qa tests.  
    Each module contains a soft link to _gnuComponentWrappers.py to pull needed components

5. sources.py and sinks.py:
   - located in gnuhawk/qa/gnuradio/gr, these modules contain overloaded versions 
     of the vector source and sink classes called in the qa tests.  These versions 
     use the sandbox source and sink classes.

6. top_block.py:
   - located in gnuhawk/qa/gnuradio/gr, this modules is used to make
     connections between blocks and start the blocks. 

--------------------------------------------
To run a qa test with the original gnuradio block:

Do the following:

1. Build the gnuradio baseline separate from what was built for gnuhawk
   A. You may need to generate an include file which is needed for the make,
pmt_unv.h
   B. You may need to install swig.
      Do the following to generate:
      1. cd <path to gnuradio source>/gruel/src/lib/pmt
      2. python generate_unv.py

   B. Create a sepate build directory for the baseline, do a cmake in that
directory pointing to the original gnuradio source, and then make
      $ mkdir $(builddir)
      $ cd $(builddir)
      $ cmake $(srcdir)
      $ make

   C. At the top of the qa test (you may want to rename or save it off before
      modifying), add the following lines to have the python script point to
      the correct python modules:

         import sys

          sys.path.append("<path to gnuradio source>/gnuradio-core/src/python/")
          sys.path.append("<path to gnuradio build>/gnuradio-core/src/lib/swig/")

       You may have to add other modules as needed.  For instance the
       qa_fir_filter requires the below line:
          sys.path.append("<path to gnuradio build>/gr-filter/swig")

      NOTE: you must do this before importing the gnuradio module
       
-----------------------------------------
Possible gotchas:

1. In _gnuComponentWrappers.py, the sig_source components set their property transfer_size 
   to the sampling_freq +1. In a couple of the qa tests, the sampling_freq is set 
   to 8, but the number of samples used for the test is 9.  The transfer_size is set 
   initially to prevent the sig_source from throttling and producing a delay.  One was added 
   to the transfer_size to make sure the number of samples pushed was enough.  If the qa test 
   is hanging or not producing enough data this may be the cause.

