set(classes
  ftkCriticalPointTracker2D
  ftkCriticalPointTracker2DSpacetime
  ftkCriticalPointTracker3D
  ftkLevelsetTracker2D
  ftkSpiralWoven2DSpacetimeSource
  ftkSpiralWoven2DSource
  ftkDoubleGyre2DSource
  ftkMerger2DSource
  ftkABCFlow3DSource
  ftkTornadoFlow3DSource
)

vtk_module_add_module(FTKFilters
  CLASSES ${classes})
vtk_module_link(FTKFilters
  PRIVATE ftk)

paraview_add_server_manager_xmls(XMLS 
  CriticalPointTracker2D.xml
  CriticalPointTracker2DSpacetime.xml
  CriticalPointTracker3D.xml
  LevelsetTracker2D.xml
  SpiralWoven2DSpacetimeSource.xml
  SpiralWoven2DSource.xml
  Merger2DSource.xml
  ABCFlow3DSource.xml
  TornadoFlow3DSource.xml
  DoubleGyre2DSource.xml
)
