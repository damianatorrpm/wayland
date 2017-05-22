import qbs 1.0

LiriDynamicLibrary {
    name: "Liri.WaylandServer.Private"
    targetName: "liriwaylandserverprivateplugin"

    Depends { name: "lirideployment" }
    Depends { name: "Qt"; submodules: ["gui", "gui-private", "qml", "quick", "waylandcompositor"] }

    cpp.defines: base.concat(["QT_WAYLAND_COMPOSITOR_QUICK"])

    files: ["*.cpp", "*.h"]

    Group {
        name: "QML Files"
        files: [
            "*.qml",
            "qmldir",
        ]
        fileTags: ["qml"]
    }

    Group {
        qbs.install: true
        qbs.installDir: lirideployment.qmlDir + "/Liri/WaylandServer/Private"
        fileTagsFilter: ["dynamiclibrary", "qml"]
    }
}