@echo off
cd ..
javadoc MapEditor MapEditor.Brushes MapEditor.BSP MapEditor.Commands MapEditor.CSG MapEditor.Event MapEditor.Geom MapEditor.Geom.Planar MapEditor.Geom.Splines MapEditor.Graphics MapEditor.GUI MapEditor.Maps MapEditor.Math MapEditor.Math.Matrices MapEditor.Math.Vectors MapEditor.Misc MapEditor.Test MapEditor.Textures -package -d MapEditor\docs
cd MapEditor