import sys
import random
import numpy

class Tree:
	left = None
	right = None
	number = 0

	def insert(self, number):
		if number == self.number:
			return
		elif number < self.number:
			if self.left is None:
				self.left = Tree()
				self.left.number = number
			else:
				self.left.insert(number)
		elif self.number < number and self.right is None:
			self.right = Tree()
			self.right.number = number
		else:
			self.right.insert(number)

	def count(self):
		if self.left is None:
			l, countl = 0, [0, 0, 0]
		else:
			l, countl = 1, self.left.count()

		if self.right is None:
			r, countr = 0, [0, 0, 0]
		else:
			r, countr = 1, self.right.count()

		return [1 + countl[0] + countr[0],
				l + countl[1] + countr[1],
				r + countl[2] + countr[2]]

	def gridCount(self, gridsize):
		count = self.count()
		gridCount = (gridsize ** 2) * count[0]
		gridCount -= (gridsize / 2) * count[1]
		gridCount -= ((int)(gridsize / 2.0 + .5)) * count[2]
		return gridCount

def randomTree(treesize):
	x = range(1, treesize + 1)
	random.shuffle(x)
	tree = Tree()
	tree.number = x[0]

	for i in x[1 : treesize + 1]:
		tree.insert(i)

	return tree

def buildGridFromTree(tree, gridsize):
	graph = []
	_buildGridFromTreeRec(tree, graph, gridsize, 0, [])
	return graph

def _buildGridFromTreeRec(tree, graph, gs, firstvert, shared):
	lastvert = firstvert + (gs ** 2) - len(shared)
	v = shared + range(firstvert, lastvert)

	for i in range(gs ** 2):
		if not (i + 1) % gs == 0:
			graph.append((v[i], v[i+1]))
		if not i + gridsize >= gridsize ** 2:
			graph.append((v[i], v[i + gs]))

	l = 0
	if not tree.left is None:
		l = _buildGridFromTreeRec(tree.left, graph, gridsize,
				lastvert,
				v[gridsize * (gridsize - 1):][:gridsize / 2])

	r = 0
	if not tree.right is None:
		r = _buildGridFromTreeRec(tree.right, graph, gridsize,
				lastvert + l,
				v[gridsize * (gridsize - 1):][gridsize / 2:])

	return l + r + (gs ** 2) - len(shared)

treesize = int(sys.argv[1])
gridsize = int(sys.argv[2])

if treesize < 1:
	raise Exception("Tree size must be positive.")
if gridsize < 2:
	raise Exception("Grid size must be at least 2.")

tree = randomTree(treesize)
graph = buildGridFromTree(tree, gridsize)

#
# UNCOMMENT THIS TO OUTPUT A TSEITIN PROGRAM INSTANCE
#
print "vertex(v1)."
print "capacity(v1, 1)."
for i in range(1, tree.gridCount(gridsize)):
	print "vertex(v%d)." % (i + 1)
	print "capacity(v%d, 0)." % (i + 1)

for (i, j) in graph:
	print "edge(v%d, v%d)." % (i + 1, j + 1)

#
# UNCOMMENT THIS TO OUTPUT A LARGE TWO-COLORABILITY RULE
#
#print "edge(red, green)."
#print "edge(green, red)."
#
#print "true :-",
#sep = ""
#for (i, j) in graph:
#	print sep + "edge(V%d, V%d)" % (i + 1, j + 1),
#	sep = ", "
#print "."
#
#
# UNCOMMENT THIS TO OUTPUT AN OLD-STYLE TWO-COLORABILITY ASP PROGRAM
#
#for i in range(1, tree.gridCount(gridsize)):
#	print "red(v%d) v blue(v%d)." % (i + 1, i + 1)
#
#for (i, j) in graph:
#	print ":- red(v%d), red(v%d)." % (i + 1, j + 1)
