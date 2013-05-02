VexCL
=======

VexCL is a vector expression template library for OpenCL. It has been created
for ease of OpenCL development with C++. VexCL strives to reduce amount of
boilerplate code needed to develop OpenCL applications. The library provides
convenient and intuitive notation for vector arithmetic, reduction, sparse
matrix-vector products, etc. Multi-device and even multi-platform computations
are supported. The source code of the library is distributed under very
permissive MIT license.

Doxygen-generated documentation is available at
http://ddemidov.github.io/vexcl.

Slides from VexCL-related talks:

* [Meeting C++ 2012, Dusseldorf](https://github.com/ddemidov/vexcl/blob/master/doc/mcpp_vexcl_2012_slides.pdf?raw=true)
* [SIAM CSE 2013, Boston](https://github.com/ddemidov/vexcl/blob/master/doc/vexcl_cse13_slides.pdf?raw=true)
* [FOSDEM 2013, Brussels](https://fosdem.org/2013/schedule/event/odes_cuda_opencl)

The paper [Programming CUDA and OpenCL: A Case Study Using Modern C++
Libraries](http://arxiv.org/abs/1212.6326) compares both convenience and
performance of several GPGPU libraries, including VexCL.

Table of contents
-----------------
* [Context initialization](#context-initialization)
* [Memory allocation](#memory-allocation)
* [Copies between host and devices](#copies-between-host-and-devices)
* [Vector arithmetics](#vector-arithmetics)
* [Reductions](#reductions)
* [User-defined functions](#user-defined-functions)
* [Random number generation](#random-number-generation)
* [Sparse matrix - vector products](#sparse-matrix-vector-products)
* [Stencil convolutions](#stencil-convolutions)
* [Fast Fourier Transform](#fast-fourier-transform)
* [Multivectors](#multivectors)
* [The generated kernels](#the-generated-kernels)
* [Converting generic C++ algorithms to OpenCL](#converting-generic-c-algorithms-to-opencl)
* [Custom kernels](#custom-kernels)
* [Supported compilers](#supported-compilers)

Context initialization
----------------------------

VexCL can transparently work with multiple compute devices that are present in
the system. VexCL context is initialized with a device filter, which is just a
functor that takes a reference to `cl::Device` and returns a `bool`. Several
[standard filters][filter-namespace] are provided, but one can easily add a
custom functor. Filters may be combined with logical operators. All compute
devices that satisfy the provided filter are added to the created context. In
the example below all GPU devices that support double precision arithmetics are
selected:
```C++
#include <iostream>
#include <stdexcept>
#include <vexcl/vexcl.hpp>

int main() {
    vex::Context ctx( vex::Filter::Type(CL_DEVICE_TYPE_GPU) && vex::Filter::DoublePrecision );

    if (!ctx) throw std::runtime_error("No devices available.");

    // Print out list of selected devices:
    std::cout << ctx << std::endl;
}
```

[filter-namespace]: http://ddemidov.github.io/vexcl/namespacevex_1_1Filter.html

Memory allocation
-----------------

`vex::vector<T>` class constructor accepts a `std::vector` of
`cl::CommandQueue`s. A `vex::Context` instance may be conveniently converted to
the type, but it is also possible to initialize the command queues elsewhere,
thus completely eliminating the need to create a `vex::Context`. Each command
queue in the list should uniquely identify a single compute device.

The contents of the created vector will be partitioned across all devices that
were present in the queue list.  Size of each partition will be proportional to
the device bandwidth, which is measured first time the device is used. All
vectors of the same size are guaranteed to be partitioned consistently, which
allows to minimize inter-device communication.

Copies between host and devices
-------------------------------

Vector arithmetics
------------------

Reductions
----------

User-defined functions
----------------------

Random number generation
------------------------

Sparse matrix -- vector products
--------------------------------

Stencil convolutions
--------------------

Fast Fourier Transform
----------------------

Multivectors
------------

The generated kernels
---------------------

Converting generic C++ algorithms to OpenCL
-------------------------------------------

Custom kernels
--------------

Supported compilers
-------------------

VexCL makes heavy use of C++11 features, so your compiler has to be modern
enough. The compilers that have been tested and supported:

* GCC v4.6 and higher.
* Clang v3.1 and higher.
* Microsoft Visual C++ 2010 and higher.

VexCL uses standard OpenCL bindings for C++ from Khronos group. The cl.hpp file
should be included with the OpenCL implementation on your system, but it is
also provided with the library.

----------------------------
_This work is a joint effort of [Supercomputer Center of Russian Academy of
Sciences][jscc] (Kazan branch) and [Kazan Federal University][kpfu]. It is
partially supported by RFBR grants No 12-07-0007 and 12-01-00033._

[jscc]: http://www.jscc.ru/eng/index.shtml
[kpfu]: http://www.kpfu.ru
