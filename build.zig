const Builder = @import("std").build.Builder;

pub fn build(b: *Builder) void {
    const target = b.standardTargetOptions(.{});
    const mode = b.standardReleaseOptions();

    const exe = b.addExecutable("hwFractal", null);
    exe.setTarget(target);
    exe.setBuildMode(mode);
    exe.install();
    exe.linkLibC();
    exe.linkLibCpp();
    exe.linkSystemLibrary("glew");
    exe.linkSystemLibrary("glm");
    exe.linkSystemLibrary("glfw3");
    exe.linkSystemLibrary("x11");
    exe.linkSystemLibrary("xcb");
    exe.linkSystemLibrary("xau");
    exe.linkSystemLibrary("xdmcp");
    exe.addIncludeDir("src");

    exe.addCSourceFiles(&.{
        "src/main.cpp",
        "src/application.cpp",
        "src/exceptions/runtime_exception.cpp",
        "src/util/config.cpp",
        "src/util/printer.cpp",
        "src/interfaces/core_controller.cpp",
        "src/gl/gl_controller.cpp",
        "src/gl/shaders.cpp"
    }, &.{
        "-std=c++17",
        "-Wall",
        "-W",
        "-fno-sanitize=undefined"
    });
}

