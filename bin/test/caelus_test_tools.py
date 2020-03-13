#!/usr/bin/env python
import os
import struct
import numpy as np
import gzip as gz
from itertools import islice
import struct
from collections import namedtuple


def reference_path(projectdir, ref_path):
    """
  Determine absolute path to reference case.
  """
    rel_path = os.path.relpath(os.getcwd(), projectdir)
    return os.path.join(ref_path, rel_path)


def compare_output(current, reference, skip):
    """
  Compare output files. Return True if they are identical an False otherwise.
  """
    with open(current) as f1, open(reference) as f2:
        f1 = islice(f1, skip, None)
        f2 = islice(f2, skip, None)
        return all(x == y for x, y in zip(f1, f2))


def compare_scalar_range(current, reference, tol):
    """
  Compare range for scalar fields. Return True if relative difference is less than 
  tol otherwise False.
  """
    scmin, scmax = get_scalar_range(current)
    srmin, srmax = get_scalar_range(reference)
    scrange = scmax - scmin
    srrange = srmax - srmin

    return abs(scrange - srrange) / srrange < tol


def compare_vector_norm_range(current, reference, tol):
    """
  Compare range for norm of vector fields. Return True if relative difference is less than 
  tol otherwise False.
  """
    vcmin, vcmax = get_vector_norm_range(current)
    vrmin, vrmax = get_vector_norm_range(reference)
    vcrange = vcmax - vcmin
    vrrange = vrmax - vrmin

    return abs(vcrange - vrrange) / vrrange < tol


def compare_field_diff_rms(current, reference, tol):
    """
  Compare RMS of difference between fields. Return True if RMS is less than 
  tol otherwise False.
  """
    frms = diff_field_rms(current, reference)

    return frms < tol


def get_scalar_range(name):
    """Returns the range (min, max) of the specified scalar field."""
    s = parse_internal_field(name)
    mins = min(s)
    maxs = max(s)
    return mins, maxs


def get_vector_norm_range(name):
    """Return the range (min, max) of the norm of the specified vector field."""
    v = parse_internal_field(name)
    n = []

    for x in range(0, v.shape[0]):
        n.append(np.linalg.norm(v[x]))

    np.array(n)
    minn = min(n)
    maxn = max(n)
    return minn, maxn


def diff_field_rms(f1name, f2name):
    """
  Taking the difference between two fields returning the RMS error
  """
    f1 = parse_internal_field(f1name)
    f2 = parse_internal_field(f2name)

    ferr = np.subtract(f1, f2)
    return np.sqrt(np.mean(np.square(ferr)))


"""
The functions below have been copied from the OpenFOAM Python Parser written by
XU Xianghua <dayigu at gmail dot com>

Copyright (c) 2017 dayigu

MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""

Boundary = namedtuple('Boundary', 'type, num, start, id')


def parse_field_all(fn):
    """
    parse internal field, extract data to numpy.array
    :param fn: file name
    :return: numpy array of internal field and boundary
    """
    if not os.path.exists(fn):
        print("Can not open file " + fn)
        return None
    if fn.endswith("gz"):
        with gz.open(fn, "rb") as f:
            content = f.readlines()
            return (
                parse_internal_field_content(content),
                parse_boundary_content(content),
            )
    else:
        with open(fn, "rb") as f:
            content = f.readlines()
            return (
                parse_internal_field_content(content),
                parse_boundary_content(content),
            )


def parse_internal_field(fn):
    """
    parse internal field, extract data to numpy.array
    :param fn: file name
    :return: numpy array of internal field
    """
    if not os.path.exists(fn):
        print("Can not open file " + fn)
        return None
    if fn.endswith("gz"):
        with gz.open(fn, "rb") as f:
            content = f.readlines()
            return parse_internal_field_content(content)
    else:
        with open(fn, "rb") as f:
            content = f.readlines()
            return parse_internal_field_content(content)


def parse_internal_field_content(content):
    """
    parse internal field from content
    :param content: contents of lines
    :return: numpy array of internal field
    """
    is_binary = is_binary_format(content)
    for ln, lc in enumerate(content):
        if lc.startswith(b"internalField"):
            if b"nonuniform" in lc:
                return parse_data_nonuniform(content, ln, len(content), is_binary)
            elif b"uniform" in lc:
                return parse_data_uniform(content[ln])
            break
    return None


def parse_boundary_field(fn):
    """
    parse internal field, extract data to numpy.array
    :param fn: file name
    :return: numpy array of boundary field
    """
    if not os.path.exists(fn):
        print("Can not open file " + fn)
        return None
    with open(fn, "rb") as f:
        content = f.readlines()
        return parse_boundary_content(content)


def parse_boundary_content(content):
    """
    parse each boundary from boundaryField
    :param content:
    :return:
    """
    data = {}
    is_binary = is_binary_format(content)
    bd = split_boundary_content(content)
    for boundary, (n1, n2) in bd.items():
        pd = {}
        n = n1
        while True:
            lc = content[n]
            if b"nonuniform" in lc:
                v = parse_data_nonuniform(content, n, n2, is_binary)
                pd[lc.split()[0]] = v
                if not is_binary:
                    n += len(v) + 4
                else:
                    n += 3
                continue
            elif b"uniform" in lc:
                pd[lc.split()[0]] = parse_data_uniform(content[n])
            n += 1
            if n > n2:
                break
        data[boundary] = pd
    return data


def parse_data_uniform(line):
    """
    parse uniform data from a line
    :param line: a line include uniform data, eg. "value           uniform (0 0 0);"
    :return: data
    """
    if b"(" in line:
        return np.array([float(x) for x in line.split(b"(")[1].split(b")")[0].split()])
    return float(line.split(b"uniform")[1].split(b";")[0])


def parse_data_nonuniform(content, n, n2, is_binary):
    """
    parse nonuniform data from lines
    :param content: data content
    :param n: line number
    :param n2: last line number
    :param is_binary: binary format or not
    :return: data
    """
    num = int(content[n + 1])
    if not is_binary:
        if b"scalar" in content[n]:
            data = np.array([float(x) for x in content[n + 3:n + 3 + num]])
        else:
            data = np.array(
                [ln[1:-2].split() for ln in content[n + 3:n + 3 + num]], dtype=float
            )
    else:
        nn = 1
        if b"vector" in content[n]:
            nn = 3
        elif b"symmtensor" in content[n]:
            nn = 6
        elif b"tensor" in content[n]:
            nn = 9
        buf = b"".join(content[n + 2:n2 + 1])
        vv = np.array(
            struct.unpack(
                "{}d".format(num * nn),
                buf[
                    struct.calcsize("c"):num * nn * struct.calcsize("d")
                    + struct.calcsize("c")
                ],
            )
        )
        if nn > 1:
            data = vv.reshape((num, nn))
        else:
            data = vv
    return data


def split_boundary_content(content):
    """
    split each boundary from boundaryField
    :param content:
    :return: boundary and its content range
    """
    bd = {}
    n = 0
    in_boundary_field = False
    in_patch_field = False
    current_path = ""
    while True:
        lc = content[n]
        if lc.startswith(b"boundaryField"):
            in_boundary_field = True
            if content[n + 1].startswith(b"{"):
                n += 2
                continue
            elif content[n + 1].strip() == b"" and content[n + 2].startswith(b"{"):
                n += 3
                continue
            else:
                print("no { after boundaryField")
                break
        if in_boundary_field:
            if lc.rstrip() == b"}":
                break
            if in_patch_field:
                if lc.strip() == b"}":
                    bd[current_path][1] = n - 1
                    in_patch_field = False
                    current_path = ""
                n += 1
                continue
            if lc.strip() == b"":
                n += 1
                continue
            current_path = lc.strip()
            if content[n + 1].strip() == b"{":
                n += 2
            elif content[n + 1].strip() == b"" and content[n + 2].strip() == b"{":
                n += 3
            else:
                print("no { after boundary patch")
                break
            in_patch_field = True
            bd[current_path] = [n, n]
            continue
        n += 1
        if n > len(content):
            if in_boundary_field:
                print("error, boundaryField not end with }")
            break

    return bd


def is_binary_format(content, maxline=20):
    """
    parse file header to judge the format is binary or not
    :param content: file content in line list
    :param maxline: maximum lines to parse
    :return: binary format or not
    """
    for lc in content[:maxline]:
        if b"format" in lc:
            if b"binary" in lc:
                return True
            return False
    return False


def is_integer(s):
    try:
        x = int(s)
        return True
    except ValueError:
        return False


class CaelusMesh(object):
    """ CaelusMesh class """
    def __init__(self, path):
        self.path = os.path.join(path, "constant/polyMesh/")
        self._parse_mesh_data(self.path)
        self.num_point = len(self.points)
        self.num_face = len(self.owner)
        self.num_inner_face = len(self.neighbour)
        self.num_cell = max(self.owner)
        self._set_boundary_faces()
        self._construct_cells()

    def cell_neighbour_cells(self, i):
        """
        return neighbour cells of cell i
        :param i: cell index
        :return: neighbour cell list
        """
        return self.cell_neighbour[i]

    def is_cell_on_boundary(self, i, bd=None):
        """
        check if cell i is on boundary bd
        :param i: cell index, 0<=i<num_cell
        :param bd: boundary name, byte str
        :return: True or False
        """
        if i < 0 or i >= self.num_cell:
            return False
        if bd is not None:
            try:
                bid = self.boundary[bd].id
            except KeyError:
                return False
        for n in self.cell_neighbour[i]:
            if bd is None and n < 0:
                return True
            elif bd and n == bid:
                return True
        return False

    def is_face_on_boundary(self, i, bd=None):
        """
        check if face i is on boundary bd
        :param i: face index, 0<=i<num_face
        :param bd: boundary name, byte str
        :return: True or False
        """
        if i < 0 or i >= self.num_face:
            return False
        if bd is None:
            if self.neighbour[i] < 0:
                return True
            return False
        try:
            bid = self.boundary[bd].id
        except KeyError:
            return False
        if self.neighbour[i] == bid:
            return True
        return False

    def boundary_cells(self, bd):
        """
        return cell id list on boundary bd
        :param bd: boundary name, byte str
        :return: cell id generator
        """
        try:
            b = self.boundary[bd]
            return (self.owner[f] for f in range(b.start, b.start+b.num))
        except KeyError:
            return ()

    def _set_boundary_faces(self):
        """
        set faces' boundary id which on boundary
        :return: none
        """
        self.neighbour.extend([-10]*(self.num_face - self.num_inner_face))
        for b in self.boundary.values():
            self.neighbour[b.start:b.start+b.num] = [b.id]*b.num

    def _construct_cells(self):
        """
        construct cell faces, cell neighbours
        :return: none
        """
        cell_num = max(self.owner) + 1
        self.cell_faces = [[] for i in range(cell_num)]
        self.cell_neighbour = [[] for i in range(cell_num)]
        for i, n in enumerate(self.owner):
            self.cell_faces[n].append(i)
        for i, n in enumerate(self.neighbour):
            if n >= 0:
                self.cell_faces[n].append(i)
                self.cell_neighbour[n].append(self.owner[i])
            self.cell_neighbour[self.owner[i]].append(n)

    def _parse_mesh_data(self, path):
        """
        parse mesh data from mesh files
        :param path: path of mesh files
        :return: none
        """
        self.boundary = self.parse_mesh_file(os.path.join(path, 'boundary'), self.parse_boundary_content)
        self.points = self.parse_mesh_file(os.path.join(path, 'points'), self.parse_points_content)
        self.faces = self.parse_mesh_file(os.path.join(path, 'faces'), self.parse_faces_content)
        self.owner = self.parse_mesh_file(os.path.join(path, 'owner'), self.parse_owner_neighbour_content)
        self.neighbour = self.parse_mesh_file(os.path.join(path, 'neighbour'), self.parse_owner_neighbour_content)

    @classmethod
    def parse_mesh_file(cls, fn, parser):
        """
        parse mesh file
        :param fn: boundary file name
        :param parser: parser of the mesh
        :return: mesh data
        """
        try:
            with open(fn, "rb") as f:
                content = f.readlines()
                return parser(content, is_binary_format(content))
        except FileNotFoundError:
            print('file not found: %s'%fn,'trying gzipped version')
            try: 
                with gz.open(fn+'.gz', "rb") as f:
                    content = f.readlines()
                    return parser(content, is_binary_format(content))
            except FileNotFoundError:
                print('file not found: %s'%fn+'.gz')
                return None

    @classmethod
    def parse_points_content(cls, content, is_binary, skip=13):
        """
        parse points from content
        :param content: file contents
        :param is_binary: binary format or not
        :param skip: skip lines
        :return: points coordinates as numpy.array
        """
        n = skip
        while n < len(content):
            lc = content[n]
            if is_integer(lc):
                num = int(lc)
                if not is_binary:
                    data = np.array([ln[1:-2].split() for ln in content[n + 2:n + 2 + num]], dtype=float)
                else:
                    buf = b''.join(content[n+1:])
                    disp = struct.calcsize('c')
                    vv = np.array(struct.unpack('{}d'.format(num*3),
                                                buf[disp:num*3*struct.calcsize('d') + disp]))
                    data = vv.reshape((num, 3))
                return data
            n += 1
        return None


    @classmethod
    def parse_owner_neighbour_content(cls, content, is_binary, skip=13):
        """
        parse owner or neighbour from content
        :param content: file contents
        :param is_binary: binary format or not
        :param skip: skip lines
        :return: indexes as list
        """
        n = skip
        while n < len(content):
            lc = content[n]
            if is_integer(lc):
                num = int(lc)
                if not is_binary:
                    data = [int(ln) for ln in content[n + 2:n + 2 + num]]
                else:
                    buf = b''.join(content[n+1:])
                    disp = struct.calcsize('c')
                    data = struct.unpack('{}i'.format(num),
                                         buf[disp:num*struct.calcsize('i') + disp])
                return list(data)
            n += 1
        return None

    @classmethod
    def parse_faces_content(cls, content, is_binary, skip=13):
        """
        parse faces from content
        :param content: file contents
        :param is_binary: binary format or not
        :param skip: skip lines
        :return: faces as list
        """
        n = skip
        while n < len(content):
            lc = content[n]
            if is_integer(lc):
                num = int(lc)
                if not is_binary:
                    data = [[int(s) for s in ln[2:-2].split()] for ln in content[n + 2:n + 2 + num]]
                else:
                    buf = b''.join(content[n+1:])
                    disp = struct.calcsize('c')
                    idx = struct.unpack('{}i'.format(num), buf[disp:num*struct.calcsize('i') + disp])
                    disp = 3*struct.calcsize('c') + 2*struct.calcsize('i')
                    pp = struct.unpack('{}i'.format(idx[-1]),
                                       buf[disp+num*struct.calcsize('i'):
                                           disp+(num+idx[-1])*struct.calcsize('i')])
                    data = []
                    for i in range(num - 1):
                        data.append(pp[idx[i]:idx[i+1]])
                return data
            n += 1
        return None

    @classmethod
    def parse_boundary_content(cls, content, is_binary=None, skip=13):
        """
        parse boundary from content
        :param content: file contents
        :param is_binary: binary format or not, not used
        :param skip: skip lines
        :return: boundary dict
        """
        bd = {}
        num_boundary = 0
        n = skip
        bid = 0
        in_boundary_field = False
        in_patch_field = False
        current_patch = b''
        current_type = b''
        current_nFaces = 0
        current_start = 0
        while True:
            if n > len(content):
                if in_boundary_field:
                    print('error, boundaryField not end with )')
                break
            lc = content[n]
            if not in_boundary_field:
                if is_integer(lc.strip()):
                    num_boundary = int(lc.strip())
                    in_boundary_field = True
                    if content[n + 1].startswith(b'('):
                        n += 2
                        continue
                    elif content[n + 1].strip() == b'' and content[n + 2].startswith(b'('):
                        n += 3
                        continue
                    else:
                        print('no ( after boundary number')
                        break
            if in_boundary_field:
                if lc.startswith(b')'):
                    break
                if in_patch_field:
                    if lc.strip() == b'}':
                        in_patch_field = False
                        bd[current_patch] = Boundary(current_type, current_nFaces, current_start, -10-bid)
                        bid += 1
                        current_patch = b''
                    elif b'nFaces' in lc:
                        current_nFaces = int(lc.split()[1][:-1])
                    elif b'startFace' in lc:
                        current_start = int(lc.split()[1][:-1])
                    elif b'type' in lc:
                        current_type = lc.split()[1][:-1]
                else:
                    if lc.strip() == b'':
                        n += 1
                        continue
                    current_patch = lc.strip()
                    if content[n + 1].strip() == b'{':
                        n += 2
                    elif content[n + 1].strip() == b'' and content[n + 2].strip() == b'{':
                        n += 3
                    else:
                        print('no { after boundary patch')
                        break
                    in_patch_field = True
                    continue
            n += 1

        return bd
