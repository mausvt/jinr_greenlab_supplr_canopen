import os

import setuptools
from setuptools import find_packages


setuptools.setup(
    name="supplr",
    version="0.0.1",
    packages=find_packages(),
    entry_points={
        "console_scripts": [
            "supplr = supplr.supplr:supplr",
        ]
    },
    description="CLI for client server application supplr CanOpen",
    author="Green Lab",
    classifiers=["Intended Audience :: Information Technology",
                 "Operating System :: POSIX :: Linux",
                 "Programming Language :: Python",
                 "Programming Language :: Python :: 3",
                 "Programming Language :: Python :: 3.6"]
)
