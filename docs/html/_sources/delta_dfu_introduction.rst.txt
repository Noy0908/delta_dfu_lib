.. _delta dfu introduction:

Delta DFU Introduction
###########################

Delta upgrade is know as differential compression. Differential
Compression is the process of creating and applying delta patch. The
ideal patch is minimal in space complexity and has high usability, so
the patch file must be easy to compress; When application applying, we
need to decompress the patch first, then combine the source image to
generate a new image. This implies that finding an algorithm for
differential compression includes diff algorithm, compress/decompress
algorithm.

.. -algorithms:

Algorithms
===============

A diff algorithm outputs the set of differences between two inputs which
is called *patch* or *delta*, it opens the door to using compression
algorithms to generate a diff and delta file which can greatly reduced
size of the delta file. Data compression is a process used to encode
information using fewer bits than the origina representation. It comes
in the forms of lossy and lossless compression. Lossless data
compression means that the compressed data will contain the same
information as the uncompressed version, which enables the compression
to be reversed. For delta updates, this is obviously needed, as the
point of the process is to recreate the target.

--------------

.. -bsdiff:

BSDiff
===========

The `BSDiff <http://www.daemonology.net/bsdiff/?ref=hackernoon.com>`__
algorithm belongs to the block move family and is focused on achieving
minimal delta/patch size. It is also specifically optimized for
executable files. A major advantage of the
`BSDiff <http://www.daemonology.net/bsdiff/?ref=hackernoon.com>`__
algorithm is that it offers a solution to the pointer problem, which is
a commonly occurring problem when creating binary patches. The pointer
problem arises from the nature of the executable file. Small, one-line,
source code adjustments to the code or data changes the relative
positions of blocks, which makes all pointers jumping over the modified
regions outdated, if not dealt with, it will creates unnecessarily large
patches. BSDiff exploits two facts to solve the pointer problem.
Firstly, that the changes outside the modified region generally will be
rather sparse, and mostly concern the least significant one or two byte.
Secondly, that data and code usually moves around in blocks, which leads
to a large number of nearby pointers having to be adjusted by the same
amount. This information can be used to deduce that the byte-wise
differences between old binary and the new are highly compressible.

--------------

.. -heat-shrink:

heat-shrink
================

`heat-shrink <https://github.com/atomicobject/heatshrink>`__ is an
excellent compression/decompression algorithm with the following
advantages:

-  **Low memory usage (as low as 50 bytes)** It is useful for some cases
   with less than 50 bytes, and useful for many general cases with < 300
   bytes.

-  **Incremental, bounded CPU use** You can chew on input data in
   arbitrarily tiny bites. This is a useful property in hard real-time
   environments.

-  **Can use either static or dynamic memory allocation** The library
   doesn't impose any constraints on memory management.

-  **ISC license** You can use it freely, even for commercial purposes.

Our project port
`heat-shrink <https://github.com/atomicobject/heatshrink>`__ algorithm
to compress/decompress the patch. it is based on
`LZSS <http://en.wikipedia.org/wiki/Lempel-Ziv-Storer-Szymanski>`__
.LZSS is a dictionary coding technique, which means it replaces a string
with a reference to a dictionary location of the same string. The idea
is to record the locations of previously unseen sub-strings. Sub-strings
which, once seen in the text again, are replaced by a pointer to the
first occurrence of this sub-string

--------------

.. -patching:

Patching
=============

In the same manner that the creation of a patch is related to data
compression, the application of a patch is related to decompression. The
patch created by the `detools <https://github.com/eerimoq/detools>`__
differencing algorithm is a sequential patch, and thus has a repeating
layout . Sequential patching uses two memory regions, one containing the
source and one containing the target, but in our implement, target is a
patch file instead of a whole firmware. In general each sequence
consists of the parts: *diff*, *extra*, and *adjustment*, which together
make up the instructions for a small section of the target image.
Sequential patching is hence a loop which repeats the same three steps
until the patch is fully applied.

--------------
