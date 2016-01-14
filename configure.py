# -*- coding: utf-8 -*-

# --------------------------------------------------------------------
#                   EasyPR auto configure script
# --------------------------------------------------------------------
#
# This script configures OpenCV3.1 for Visual Studio
# on Windows.
#
# You are required to have Python3.* installed, and python.exe must
# be added to your PATH (C:\Python34 for example).
#
# You can use it by executing:
#
#     C:\> cd path\to\EasyPR
#     C:\> python configure.py
#
# Note: compatible with python3, haven't been tested on python2.
#
# --------------------------------------------------------------------

import os
import re

kProjectDir = "vcprojs"

kProjects = ["libeasypr.vcxproj", "demo.vcxproj"]

kProjectTemplates = ["libeasypr.vcxproj.template", "demo.vcxproj.template"]

kOpenCVConfig = "OpenCVConfig-version.cmake"

kConfig = {
    "build": "",
    "include": "",
    "library": "",
    "link": ["opencv_world310"],
    "bit": "",
    "vs": ""
}

kPatterns = {
    "include": "(<AdditionalIncludeDirectories>)(.*?)(</AdditionalIncludeDirectories>)",
    "library": "(<AdditionalLibraryDirectories>)(.*?)(</AdditionalLibraryDirectories>)",
    "link": "(<AdditionalDependencies>)(.*?)(</AdditionalDependencies>)"
}

kReplacements = {
    "include": r"\1%s;\2\3",
    "library": r'\1%s\3',
    "link": r'\1%s;\2\3'
}


def configure():
    for i in range(2):
        print(">> creating %s" % kProjects[i])
        tpath = os.path.join(kProjectDir, kProjectTemplates[i])
        fp = open(tpath, encoding="utf-8")
        try:
            # read from disk
            original = fp.read()
            nstring = ""
            if 0 == i:
                nstring = configure_libeasypr(original)
            elif 1 == i:
                nstring = configure_demo(original)

            # write to disk
            wpath = os.path.join(kProjectDir, kProjects[i])
            writer = open(wpath, mode="wb")
            try:
                writer.write(nstring.encode())
            finally:
                writer.close()
        finally:
            fp.close()
    print(">> all done! Open EasyPR.sln and have fun!")


def configure_libeasypr(buffer):
    # additional include dir
    pattern = re.compile(kPatterns["include"])
    return pattern.sub(kReplacements["include"] %
                       (kConfig["include"][:2] + re.escape(kConfig["include"][2:])),
                       buffer)


def configure_demo(buffer):
    # additional include dir
    pattern = re.compile(kPatterns["include"])
    nstring = pattern.sub(kReplacements["include"] %
                          (kConfig["include"][:2] + re.escape(kConfig["include"][2:])),
                          buffer)
    # additional library dir
    pattern = re.compile(kPatterns["library"])
    nstring = pattern.sub(kReplacements["library"] %
                          (kConfig["library"][:2] + re.escape(kConfig["library"][2:])),
                          nstring)
    # additional dependencies
    #lib_string = ""
    #for lib in kConfig["link"]:
    #    lib_string += (lib + "d.lib")

    #pattern = re.compile(kPatterns["link"])
    #return pattern.sub(kReplacements["link"] % lib_string, nstring)

    return nstring


def check_opencv_version():
    file = os.path.join(kConfig["build"], kOpenCVConfig)
    print(">> Checking ", file)
    fp = open(file)
    opencv_version = 0
    try:
        fline = fp.readline()
        match = re.search(r"OpenCV_VERSION (\d)\.(\d)\.(\d{,2})", fline)
        if match is not None:
            opencv_version = match.group(1) + "." + match.group(2)
    finally:
        fp.close()
    return opencv_version


def cli():
    while True:
        root_ = input(r"Where is your opencv root path? (e.g, C:\path\to\opencv3): ")
        if os.path.exists(root_):
            kConfig["build"] = os.path.join(root_, "build")
            kConfig["include"] = os.path.join(kConfig["build"], "include")
            break
        else:
            print("Invalid path")

    if check_opencv_version() != "3.1":
        print("requires opencv 3.1")
        exit()

    kConfig["bit"] = "x64"

    while True:
        vc = input("Which Visual Studio you are using? (vs2013 or vs2015): ")
        if vc == "vs2013":
            kConfig["vs"] = "vc12"
            break
        elif vc == "vs2015":
            kConfig["vs"] = "vc14"
            break
        else:
            print("Please type vs2013 or vs2015")

    kConfig["library"] = os.path.normpath("%s/%s/%s/lib/" % (kConfig["build"], kConfig["bit"], kConfig["vs"]))


if __name__ == "__main__":
    cli()
    configure()
