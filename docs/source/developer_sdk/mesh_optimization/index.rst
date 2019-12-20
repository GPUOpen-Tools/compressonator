Mesh Optimization
=================

Mesh optimization is only supported on Windows platform. The SDK is still under development and work is in progress on packaging it into a single lib and header for distribution.
Currently, to use mesh optimization you will need to first build a library using the `mesh_optimizer.vcxproj <https://github.com/GPUOpen-Tools/Compressonator/tree/master/Compressonator/Applications/_Plugins/CMesh/mesh_optimizer>`_.
This build is also dependent on `CMP_MeshOptimizer.vcxproj <https://github.com/GPUOpen-Tools/Compressonator/tree/master/Compressonator/VS2015>`_ .

In your code, link in the built libraries and include the header file `mesh_optimizer.h <https://github.com/GPUOpen-Tools/Compressonator/blob/master/Compressonator/Applications/_Plugins/CMesh/mesh_optimizer/mesh_optimizer.h>`_, which defines a class interface for mesh optimization processing:

 ``ProcessMesh(void* data, void* setting, void* statsOut, CMP_Feedback_Proc pFeedbackProc)`` 

* void* data is CMODEL_DATA type from `ModelData.h <https://github.com/GPUOpen-Tools/Compressonator/blob/master/Compressonator/Applications/_Plugins/Common/ModelData.h>`_ .
* void* setting is MeshSettings type from `mesh_optimizer.h <https://github.com/GPUOpen-Tools/Compressonator/blob/master/Compressonator/Applications/_Plugins/CMesh/mesh_optimizer/mesh_optimizer.h>`_.
* void* statsOut and CMP_Feedback_Proc pFeedbackProc are reserved for statistic output and progress feedbacks, they are optional input and can be nullptr.

.. toctree::
   :maxdepth: 8
   :name: toc-developer_sdk-mesh_optimization
   


