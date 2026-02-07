# Example Testing
This project runs as part of the CI/CD pipeline for the library. This ensures that the examples are always up to date with the library itself.

## How to add an example
One project is used to build all of the examples, and this works by utilising different 'build configurations'. In a standard project this is usually just 'Debug' and 'Release', but here we have separate configurations that correspond to compiling each example. To add a new example:
- Add a file into the `examples/` folder.
- Open this project in CCS, and for each existing build configuration (Project > Build Configuration), right click the new file in the examples folder and hit 'exclude from build' to make sure the file isn't dragged into other examples. 
- Next add a new build configuration (Project > Build Configuration > Manage). In the examples folder, select all but the new file, and right click and select 'exclude from build' so only the new file is included in the build configuration you added.
- Then add a line to `.github/workflows/build.yaml` that compiles the project with the new build configuration.
