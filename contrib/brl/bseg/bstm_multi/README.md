# Multi-Level BSTM

Multi-level BSTM is a data structure for representing time-varying
voxel data (e.g. 3D video). It is an extension of the BSTM data
structure, which can be found in [this repository (../bstm/)](../bstm/). A
detailed explanation of BSTM is given in [Ulusoy et al, 2013](http://www.cv-foundation.org/openaccess/content_iccv_2013/papers/Ulusoy_Dynamic_Probabilistic_Volumetric_2013_ICCV_paper.pdf).

## Data Structure

## Implementation Details

### File structure

In general, files prefixed `bstm_multi_` are specific to the
multi-BSTM data structure. Files prefixed `space_time_` contain
generic data structures for 4D scenes. Files starting with `block_`
are for managing generic blocks, regardless of whether or not these
correspond to a 4D scene or any other type of scene.

## TODO

Multi-BSTM is still incomplete. The following needs to be done:
- Basic processes for creating and managing scenes
- Code & processes for conversion of BOXM2/BSTM scenes into Multi-BSTM
- Code & processes for rendering
- Benchmarks comparing performance to BSTM

### bugs/notes/etc

 - Currently, BSTM ingestion requires that multi-BSTM blocks have same size as BSTM blocks. This means that BSTM block sizes (i.e. number of sub-blocks) must be a power of 512 (i.e. 8 in each dimension). Similarly, the number of frames should be a power of 32.

 - use namespaces instead of prefixes everywhere?

## Contact

Raphael Kargon <raphael.kargon@gmail.com>
