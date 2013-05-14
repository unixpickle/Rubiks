corners = range(0, 8)
edges = range(0, 12)
edgesX = [2, 11, 8, 10, 3, 1, 0, 5, 6, 4, 9, 7]
edgesY = [5, 7, 11, 1, 0, 6, 4, 9, 10, 3, 2, 8]
edgesZ = [3, 0, 1, 2, 10, 4, 9, 6, 7, 8, 11, 5]

def rotateElem(point):
    if point == (0, 0): return (1, 0)
    if point == (1, 0): return (1, 1)
    if point == (1, 1): return (0, 1)
    return (0, 0)

def rotateEdges(edges, count=1, axis='x'):
    if count == 0: return edges
    global edgesX, edgesY, edgesZ
    newEdges = []
    rotation = edgesX
    if axis == 'y': rotation = edgesY
    if axis == 'z': rotation = edgesZ
    for i in range(0, 12):
        newEdges.append(edges[rotation[i]])
    return rotateEdges(newEdges, count - 1, axis)

def rotateCorners(corners, count=1, axis='x'):
    if count == 0: return corners
    aMask = 1
    bMask = 2
    if axis == 'y':
        bMask = 4
    elif axis == 'z':
        aMask = 4
    newCorners = []
    for i in range(0, 8):
        corner = corners[i]
        a = 0 if corner & aMask == 0 else 1
        b = 0 if corner & bMask == 0 else 1
        rotated = rotateElem((a, b))
        corner &= 0x7 ^ (aMask | bMask)
        if rotated[0]: corner |= aMask
        if rotated[1]: corner |= bMask
        newCorners.append(corner)
    return rotateCorners(newCorners, count - 1, axis)

def printEdgeCornerSymmetry(edge, corner):
    desc = '  {{'
    for i in range(0, 12):
        desc += str(edge[i])
        if i != 11: desc += ', '
    desc += '}, {'
    for i in range(0, 8):
        desc += str(corner[i])
        if i != 7: desc += ', '
    desc += '}},'
    print desc

def main():
    global edges, corners
    edgeList = []
    cornerList = []
    edgeFront = edges
    edgeList.append(edgeFront)
    edgeList.append(rotateEdges(edgeFront, 2, 'y'))
    edgeList.append(rotateEdges(edgeFront, 1, 'x'))
    edgeList.append(rotateEdges(edgeFront, 3, 'x'))
    edgeList.append(rotateEdges(edgeFront, 1, 'y'))
    edgeList.append(rotateEdges(edgeFront, 3, 'y'))
    
    cornerFront = corners
    cornerList.append(cornerFront)
    cornerList.append(rotateCorners(cornerFront, 2, 'y'))
    cornerList.append(rotateCorners(cornerFront, 1, 'x'))
    cornerList.append(rotateCorners(cornerFront, 3, 'x'))
    cornerList.append(rotateCorners(cornerFront, 1, 'y'))
    cornerList.append(rotateCorners(cornerFront, 3, 'y'))
    
    print "All 24 rotational symmetries:"
    
    for i in range(0, 6):
        for j in range(0, 4):
            symEdges = rotateEdges(edgeList[i], j, 'z')
            symCorners = rotateCorners(cornerList[i], j, 'z')
            printEdgeCornerSymmetry(symEdges, symCorners)
    
    print "8 rotational y-axis symmetries"
    for i in range(0, 4):
        symEdges = rotateEdges(edges, i, 'y')
        symCorners = rotateCorners(corners, i, 'y')
        for j in range (0, 2):
            symEdges = rotateEdges(symEdges, j*2, 'z')
            symCorners = rotateCorners(symCorners, j*2, 'z')
            printEdgeCornerSymmetry(symEdges, symCorners)

main()
    
