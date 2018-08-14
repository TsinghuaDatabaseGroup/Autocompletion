
// $Id: ActiveNode.h 638 2010-11-28 14:49:05Z liguoliang $

#ifndef __ACTIVENODE_H__
#define __ACTIVENODE_H__

#include "Trie.h"

#include <vector>
#include <map>
#include <set>
#include <iostream>


/*
PAN:
*/
typedef struct _PivotalActiveNode
{
	unsigned transformationdistance;
	unsigned short differ; // |p_{x+1}-p_i|
	unsigned short editdistanceofPrefix;
}PivotalActiveNode;


/*
typedef TrieNode<charT> TrieNode_Internal;
typedef Trie<charT> Trie_Internal;
 */

template<class charT>
struct ResultNode {
	const TrieNode<charT> *node;
	int editDistance;
	int prefixLength;
	ResultNode(TrieNode<charT>* in_node):node(in_node){}
	ResultNode(const TrieNode<charT>* in_node, int in_editDistance, int in_prefixLength):node(in_node),
			editDistance(in_editDistance),prefixLength(in_prefixLength){}
};


template<class charT>
class PrefixActiveNodeSet
{
public:
	typedef std::vector<TrieNode<charT>* > TrieNodeSet;

	void init(std::string &prefix, const unsigned editDistanceThreshold) {
		this->prefix = prefix;
		this->editDistanceThreshold = editDistanceThreshold;

		this->trieNodeSetVector.clear();
		this->trieNodeSetVectorComputed = false;

		this->flagResultsCached = false;
	}


	PrefixActiveNodeSet(std::string &prefix, const unsigned editDistanceThreshold) {
		init(prefix, editDistanceThreshold);
	}

	/// A set of active nodes for an empty string and an edit-distance threshold
	PrefixActiveNodeSet(Trie<charT> *trie, const unsigned editDistanceThreshold) {
		std::string emptyString = "";
		init(emptyString, editDistanceThreshold);

		//PAN:
		// Add the trie nodes up to the given depth
		TrieNode<charT> *root = trie->getRoot();
		_addActiveNode(root, 0); // add the root first
		PivotalActiveNode pan;
		pan.transformationdistance = 0;
		pan.differ = 0; // |p_{x+1}-p_i|
		pan.editdistanceofPrefix = 0;
		_addPAN(root, pan);
		//if (editDistanceThreshold > 0)
		//	addTrieNodesUpToDepth(root, editDistanceThreshold, 0);

		
	}

	~PrefixActiveNodeSet() {
	}

	PrefixActiveNodeSet<charT> *computeActiveNodeSetIncrementally(const char additionalChar) {
		// form the new string. // TODO (OPT): avoid string copy
		std::string newString = this->prefix;
		newString.push_back(additionalChar);

		PrefixActiveNodeSet<charT> *newActiveNodeSet = new PrefixActiveNodeSet<charT>(newString, this->getEditDistanceThreshold());

		// PAN:
		for (typename std::map<TrieNode<charT>*, PivotalActiveNode >::iterator mapIterator = PANMap.begin();
		     mapIterator != PANMap.end(); mapIterator ++) {
			// Compute the new active nodes for this trie node
			_addPANSetForOneNode(mapIterator->first, mapIterator->second, additionalChar, newActiveNodeSet);
		}
		/* iterate over all the active nodes
		for (typename std::map<TrieNode<charT>*, unsigned >::iterator mapIterator = trieNodeDistanceMap.begin();
		     mapIterator != trieNodeDistanceMap.end(); mapIterator ++) {
			// Compute the new active nodes for this trie node
			_addActiveNodeSetForOneNode(mapIterator->first, mapIterator->second, additionalChar, newActiveNodeSet);
		}*/


		//PAN: update active node
		for (typename std::map<TrieNode<charT>*, PivotalActiveNode >::iterator mIterator = newActiveNodeSet->PANMap.begin();
		     mIterator != newActiveNodeSet->PANMap.end(); mIterator ++) {
			newActiveNodeSet->_addActiveNode(mIterator->first, mIterator->second.transformationdistance);
		}
		return newActiveNodeSet;
	}

	bool  fillResultNodeSet(std::vector<ResultNode<charT> > &result,int size = -1) const {
		return 0;
	}

	unsigned getEditDistanceThreshold() const {
		return editDistanceThreshold;
	}

	unsigned getNumberOfBytes() const {

		unsigned trieNodeSetVectorSize = 0;

        for ( typename std::vector<TrieNodeSet>::const_iterator vectorIter = trieNodeSetVector.begin(); vectorIter != trieNodeSetVector.end(); vectorIter++ )
        {
        	trieNodeSetVectorSize += (*vectorIter).capacity() * sizeof(*vectorIter);
        }
        trieNodeSetVectorSize += trieNodeSetVector.size() * sizeof(TrieNodeSet)
        		+ (this->trieNodeSetVector.capacity() - trieNodeSetVector.size()) * sizeof(void*);

		return this->prefix.size()
				+ sizeof(this->editDistanceThreshold)
				+ trieNodeDistanceMap.size() * (sizeof(void*) + sizeof(unsigned)) + sizeof(trieNodeDistanceMap)
				+ trieNodeSetVectorSize;
	}

	unsigned getNumberOfActiveNodes() {
		return (unsigned) trieNodeDistanceMap.size();
	}

	std::string *getPrefix() {
		return &prefix;
	}

	unsigned getPrefixLength() const {
		return prefix.length();
	}

	void getComputedSimilarPrefixes(std::vector<std::string> &similarPrefixes) {
		for (typename std::map<TrieNode<charT>*, unsigned >::iterator mapIterator = trieNodeDistanceMap.begin();
				mapIterator != trieNodeDistanceMap.end(); mapIterator ++) {
			TrieNode<charT> *trieNode = mapIterator->first;
			std::string prefix;
			trieNode->getPrefix(prefix);
			similarPrefixes.push_back(prefix);
		}
	}

	//typedef std::vector<TrieNode<charT>* > TrieNodeSet;
	std::vector<TrieNodeSet> *getTrieNodeSetVector() {

		// compute it only if necessary
		if (this->trieNodeSetVectorComputed)
			return &trieNodeSetVector;

		_computeTrieNodeSetVector();
		return &trieNodeSetVector;
	}

	// add the descendants of the current node up to "depth" to the trieNodeDistanceMap with
	// the corresponding edit distance.  The edit distance of the current node is "editDistance".
	void addTrieNodesUpToDepth(TrieNode<charT> *trieNode, const unsigned depthLimit, const unsigned editDistance) {
		unsigned currentDdepth = trieNode->getDepth();
		// ASSERT(currentDdepth < depthLimit); // this node should have been added by its parent

		// add children
		for (unsigned int childIterator = 0; childIterator < trieNode->getChildrenCount(); childIterator++) {
			TrieNode<charT> *child = trieNode->getChild(childIterator);
			_addActiveNode(child, editDistance + 1); // add one more node
			if (currentDdepth + 1 < depthLimit) // recursive call for each child
				addTrieNodesUpToDepth(child, depthLimit, editDistance + 1);
		}
	}



	// The following function is used to implement the iterator to go through the leaf nodes.

	// Compute the set of active nodes with an edit distance to the prefix within a given range.
	// Minimize them by not including active nodes whose ancestor is already included.
	// The goal is to make it more efficient to compute their leaf nodes
	void computeMinimizedTrieNodesInRange(unsigned minEd, unsigned maxEd, std::map<TrieNode<charT>*, unsigned> &minimizedTrieNodesMap) {
		// ASSERT(minEd <= maxEd);
		// ASSERT(maxEd <= editDistanceThreshold);

		// TODO (OPT): avoid generating a temporary map
		std::map<TrieNode<charT>*, unsigned> trieNodes, *trieNodesPointer;

		// no need to compute the temporary map if the range is [0, edThreshold]
		if (minEd == 0 && maxEd == editDistanceThreshold) {
			trieNodesPointer = &(this->trieNodeDistanceMap);
		}
		else { // we need to compute it
			for (typename std::map<TrieNode<charT>*, unsigned >::const_iterator mapIterator = this->trieNodeDistanceMap.begin();
					mapIterator != this->trieNodeDistanceMap.end(); mapIterator ++) {
				if (mapIterator->second >= minEd && mapIterator->second <= maxEd)
					trieNodes[mapIterator->first] = mapIterator->second; // add it to the new map
			}
			trieNodesPointer = &(trieNodes); // pointing to the new one
		}

		// compute the minimizedTrieNodesMap
		minimizedTrieNodesMap.clear();
		for (typename std::map<TrieNode<charT>*, unsigned >::const_iterator mapIterator = trieNodesPointer->begin();
				mapIterator != trieNodesPointer->end(); mapIterator ++) {
			// add it to the map only if it doesn't have an ancestor in the set
			if (_hasAncestorInMap(trieNodesPointer, mapIterator->first) == false)
				minimizedTrieNodesMap[mapIterator->first] = mapIterator->second;
		}
	}

	bool isResultsCached() const	{
		return this->flagResultsCached;
	}

	void setResultsCached(bool flag) {
		this->flagResultsCached = flag;
	}

private:
	std::string prefix;
	unsigned editDistanceThreshold;
	bool flagResultsCached;

	// A map from trie node to its edit distance to the prefix
	std::map<TrieNode<charT>*, unsigned> trieNodeDistanceMap;

	//PAN: A map from trie node to its pivotal active nodes
	std::map<TrieNode<charT>*, PivotalActiveNode > PANMap;

	// group the trie nodes based on their edit distance to the prefix.
	// used only when it's called by an interator
	std::vector<TrieNodeSet> trieNodeSetVector;
	bool trieNodeSetVectorComputed; // indicated if the trieNodeSetVector has been computed


    //PAN:

	/// compute the pivotal active nodes based on one of the active nodes of the previous prefix
	/// add the new pivotal active nodes to newActiveNodeSet
	void _addPANSetForOneNode(TrieNode<charT> *trieNode, PivotalActiveNode pan,
			const char additionalChar, PrefixActiveNodeSet<charT> *newActiveNodeSet) {
		// deletion
		PivotalActiveNode dpan;
		dpan.transformationdistance = pan.transformationdistance + 1;
		dpan.differ = pan.differ + 1;
		dpan.editdistanceofPrefix = pan.editdistanceofPrefix;
		newActiveNodeSet->_addPAN(trieNode, dpan);

		// go through the children of this treNode
		int depthLimit = this->getEditDistanceThreshold() - pan.transformationdistance;
		int curDepth = 0;
		addPANUpToDepth(trieNode, pan, curDepth, depthLimit, additionalChar, newActiveNodeSet);
		
	}

	//PAN:
	/// Add a new pivotal active node with an edit distance.
	/// If the pivotal active node already exists in the set and had a distance no greater than the new one,
	/// then ignore this request.
	void _addPAN(TrieNode<charT> *trieNode, PivotalActiveNode pan) {
		if (pan.transformationdistance > this->editDistanceThreshold) // do nothing if the new distance is above the threshold
			return;
		//PAN:
		typename std::map<TrieNode<charT>*, PivotalActiveNode >::iterator mapIterator = PANMap.find(trieNode);
		if (mapIterator != PANMap.end()) { // found one
			if (mapIterator->second.transformationdistance > pan.transformationdistance) // reassign the distance if it's smaller
				mapIterator->second = pan;
			else if (mapIterator->second.transformationdistance == pan.transformationdistance)
			{
				if((mapIterator->second.differ < pan.differ)||(mapIterator->second.editdistanceofPrefix > pan.editdistanceofPrefix))
					mapIterator->second = pan;
			}
			return; // otherwise, do nothing
		}

		// insert the new pair
		PANMap.insert(std::pair<TrieNode<charT>*, PivotalActiveNode >(trieNode, pan));

		// set the flag
		this->trieNodeSetVectorComputed = false;
	}



	//PAN:

	// add the descendants of the current node up to "depth" to the PANMao with
	// the corresponding edit distance.  The edit distance of the current node is "editDistance".
	void addPANUpToDepth(TrieNode<charT> *trieNode, PivotalActiveNode pan, const unsigned curDepth, const unsigned depthLimit, const char additionalChar, PrefixActiveNodeSet<charT> *newActiveNodeSet) {
		// add children
		int max = curDepth;
		if( max < pan.differ )
			max = pan.differ;
		PivotalActiveNode panlocal;
		for (unsigned int childIterator = 0; childIterator < trieNode->getChildrenCount(); childIterator++) {
			TrieNode<charT> *child = trieNode->getChild(childIterator);			
			if (child->getCharacter() == additionalChar) { // match
				panlocal.transformationdistance = pan.editdistanceofPrefix + max;
				panlocal.differ = 0;
				panlocal.editdistanceofPrefix = pan.editdistanceofPrefix + max;
				newActiveNodeSet->_addPAN(child, panlocal);
			}
			if (curDepth + 1 <= depthLimit) {// recursive call for each child
				addPANUpToDepth(child, pan, curDepth+1, depthLimit, additionalChar, newActiveNodeSet);
			}
		}
	}



	/// compute the active nodes based on one of the active nodes of the previous prefix
	/// add the new active nodes to newActiveNodeSet
	void _addActiveNodeSetForOneNode(TrieNode<charT> *trieNode, unsigned distance,
			const char additionalChar, PrefixActiveNodeSet<charT> *newActiveNodeSet) {
		// deletion (from the new string's perspective
		newActiveNodeSet->_addActiveNode(trieNode, distance + 1);

		// go through the children of this treNode
		for (unsigned int childIterator = 0; childIterator < trieNode->getChildrenCount(); childIterator ++) {
			TrieNode<charT> *child = trieNode->getChild(childIterator);
			if (child->getCharacter() != additionalChar) { // substitution
				newActiveNodeSet->_addActiveNode(child, distance + 1);
			}
			else { // the two chars match
				newActiveNodeSet->_addActiveNode(child, distance);

				// Insert the descendants of this child up to a certain depth
				if (distance < editDistanceThreshold) { // do the math here
					unsigned additionalSteps = editDistanceThreshold - distance;
					newActiveNodeSet->addTrieNodesUpToDepth(child, child->getDepth() + additionalSteps, distance);
				}
			}
		}
	}



	//PAN:
	/// Add a new active node with an edit distance.
	/// If the active node already exists in the set and had a distance no greater than the new one,
	/// then ignore this request.
	void _addActiveNode(TrieNode<charT> *trieNode, unsigned editDistance) {
		if (editDistance > this->editDistanceThreshold) // do nothing if the new distance is above the threshold
			return;

		//PAN:
		typename std::map<TrieNode<charT>*, unsigned >::iterator mapIterator = trieNodeDistanceMap.find(trieNode);
		if (mapIterator != trieNodeDistanceMap.end()) { // found one
			if (mapIterator->second > editDistance) // reassign the distance if it's smaller
				mapIterator->second = editDistance;
			return; // otherwise, do nothing
		}

		// insert the new pair
		trieNodeDistanceMap.insert(std::pair<TrieNode<charT>*, unsigned>(trieNode, editDistance));

		// set the flag
		this->trieNodeSetVectorComputed = false;
	}

	void _computeTrieNodeSetVector() {
		if (this->trieNodeSetVectorComputed)
			return;

		// VECTOR: initialize the vector
		this->trieNodeSetVector.resize(editDistanceThreshold + 1);
		for (unsigned i = 0; i <= editDistanceThreshold; i++)
			this->trieNodeSetVector[i].clear();

		// go over the map to populate the vectors.
		for (typename std::map<TrieNode<charT>*, unsigned >::iterator mapIterator = trieNodeDistanceMap.begin();
				mapIterator != trieNodeDistanceMap.end(); mapIterator ++) {
			this->trieNodeSetVector[mapIterator->second].push_back(mapIterator->first);
		}

		// set the flag
		this->trieNodeSetVectorComputed = true;
	}

	// check if the given trieNode has an ancestor (not including itself) in the given set of active nodes
	bool _hasAncestorInMap(std::map<TrieNode<charT>*, unsigned> *trieNodeMap, TrieNode<charT> *trieNode) {

		for (TrieNode<charT> *node = trieNode->getParent(); node != NULL; node = node->getParent()) {
			if (trieNodeMap->find(node) != trieNodeMap->end()) // found one
				return true;
		}
		return false;
	}
};

/*
 * An iterator to go through the active nodes and their distances stored in a PrefixActiveNodeSet.
 * Design based on the example at http://sourcemaking.com/design_patterns/iterator/cpp/1
 * 
 * An example:
 * PrefixActiveNodeSet<charT> *prefixActiveNodeSet;
       for (ActiveNodeSetIterator<charT> ani(prefixActiveNodeSet); !ani.isDone(); ani.next()) {
            // get the trie node and its distance
            TrieNode<charT> *trieNode;
            unsigned distance;
            ani.getItem(trieNode, distance);
	}
 */

template<class charT>
class ActiveNodeSetIterator
{
public:
	// generate an iterator for the active nodes whose edit distance is within the given @edUpperBound
	ActiveNodeSetIterator(PrefixActiveNodeSet<charT> *prefixActiveNodeSet, const unsigned edUpperBound) {
		_initActiveNodeIterator(prefixActiveNodeSet, edUpperBound);
	}

	void next() {
		if (isDone())
			return;

		offsetCursor ++;

		if (offsetCursor < trieNodeSetVector->at(editDistanceCursor).size()) 
			return;

		// reached the tail of the current vector
		editDistanceCursor ++;
		offsetCursor = 0;

		// move editDistanceCursor to the next non-empty vector
		while (editDistanceCursor <= this->edUpperBound &&
				trieNodeSetVector->at(editDistanceCursor).size() == 0)
			editDistanceCursor ++;
	}

	bool isDone() {
		if (editDistanceCursor <= this->edUpperBound &&
				offsetCursor < trieNodeSetVector->at(editDistanceCursor).size())
			return false;

		return true;
	}

	void getItem(TrieNode<charT> *&trieNode, unsigned &distance) {
		if (isDone()) {
			trieNode = NULL;
			distance = 0;
		}
		else {
			trieNode = trieNodeSetVector->at(editDistanceCursor).at(offsetCursor);
			distance = editDistanceCursor;
		}

		//ASSERT(distance != 0);
	}

private:
	typedef std::vector<TrieNode<charT>* > TrieNodeSet;

	std::vector<TrieNodeSet> *trieNodeSetVector;
	unsigned edUpperBound;
	unsigned editDistanceCursor;
	unsigned offsetCursor;

	// initialize an iterator to store all the active nodes whose edit distance to
	// the query prefix is within the bound @edUpperBound
	void _initActiveNodeIterator(PrefixActiveNodeSet<charT> *prefixActiveNodeSet, const unsigned edUpperBound) {
		// we materialize the vector of trie nodes (indexed by the edit distance) only during the
		// phase of an iterator
		this->trieNodeSetVector = prefixActiveNodeSet->getTrieNodeSetVector();

		// ASSERT(edUpperBound < trieNodeSetVector->size());
		this->edUpperBound = edUpperBound;

		// initialize the cursors
		this->editDistanceCursor = 0;
		// Find the first valid active node
		while (editDistanceCursor <= edUpperBound &&
				trieNodeSetVector->at(editDistanceCursor).size() == 0)
			editDistanceCursor ++;

		this->offsetCursor = 0;
	}

};

/*
 * An iterator to go through the leaf nodes of the active nodes in a PrefixActiveNodeSet.
 * An example:
 * PrefixActiveNodeSet<charT> *prefixActiveNodeSet;
 *         for (LeafNodeSetIterator<charT> lnsi(prefixActiveNodeSet); !lnsi.isDone(); lnsi.next()) {
            TrieNode<charT> *prefixNode;
            TrieNode<charT> *leafode;
            unsigned distance;
            ani.getItem(prefixNode, leafNode, distance);
	}
 */

// a structure to record information of each node
template<class charT>
struct LeafNodeSetIteratorItem
{
  TrieNode<charT> *prefixNode;
  TrieNode<charT> *leafNode;
  unsigned distance;

  // TODO: OPT. Return the length of the prefix instead of the prefixNode pointer?
  LeafNodeSetIteratorItem(TrieNode<charT> *prefixNode, TrieNode<charT> *leafNode, unsigned distance) 
  {
    this->prefixNode = prefixNode;
    this->leafNode = leafNode;
    this->distance = distance;
  }

};

template<class charT>
class LeafNodeSetIterator
{
private:
	std::vector<LeafNodeSetIteratorItem<charT> > resultVector;
	unsigned cursor;

public:
	// for a set of active nodes, given a threshold edUpperBound, find
	// all the leaf nodes whose minimal edit distance (among all their prefixes)
	// is within the edUpperBound.
	// Provide an iterator that can return the leaf nodes sorted by their
	// minimal edit distance

	// Implementation: 1. Get the set of active nodes with an edit distance <= edUpperBound, sorted 
	//                 based on their edit distance,
	//                 2. get their leaf nodes, and keep track of the visited nodes so that
	//                   each node is visited only once.
	LeafNodeSetIterator(PrefixActiveNodeSet<charT> *prefixActiveNodeSet, const unsigned edUpperBound) {
	  _initLeafNodeSetIterator(prefixActiveNodeSet, edUpperBound);
	}

        void next() {
	  if (isDone())
	    return;
	  cursor ++;
        }

        bool isDone() {
	  //if (cursor >= leafNodesVector.size())
	  if (cursor >= resultVector.size())
	    return true;
	  return false;
        }

	void getItem(TrieNode<charT> *&prefixNode, TrieNode<charT> *&leafNode, unsigned &distance) {
	  if (isDone()) {
	    prefixNode = NULL;
	    leafNode = NULL;
	    distance = 0;
	  }
	  else {
	    prefixNode = resultVector.at(cursor).prefixNode;
	    leafNode = resultVector.at(cursor).leafNode;
	    distance = resultVector.at(cursor).distance;
	  }
	}

private:
        void _initLeafNodeSetIterator(PrefixActiveNodeSet<charT> *prefixActiveNodeSet, const unsigned edUpperBound) {
        	std::set<TrieNode<charT>*> visitedTrieNodes;

	  // assume the iterator returns the active nodes in an ascending order of their edit distance                                                                                                        
	  for (ActiveNodeSetIterator<charT> ani(prefixActiveNodeSet, edUpperBound); !ani.isDone(); ani.next()) {
	    // get the trie node and its distance                                                                                                                                                             
	    TrieNode<charT> *trieNode;
	    unsigned distance;
	    ani.getItem(trieNode, distance);

	    // append the leaf nodes of this active node to the vector                                                                                                                                      
	    _appendLeafNodes(trieNode, trieNode, distance, visitedTrieNodes);
	  }
	  
	  // init the cursor
	  cursor = 0;
        }

	// add the leaf nodes of the given trieNode to a vector.  Add those decendant nodes to visitedTrieNodes.
	// Ignore those decendants that are already in visitedTrieNodes
	void _appendLeafNodes(TrieNode<charT> *prefixNode, TrieNode<charT> *trieNode, unsigned editDistance, std::set<TrieNode<charT>*> &visitedTrieNodes) {
	  // do nothing if this node has already been visited
	  if (visitedTrieNodes.find(trieNode) != visitedTrieNodes.end())
	    return;

	  // mark this node visited
	  visitedTrieNodes.insert(trieNode);

	  if (trieNode->isTerminalNode()) {
		// TODO: prefix might not be unique. Should we return the longest matching prefix?
	    resultVector.push_back(LeafNodeSetIteratorItem<charT>(prefixNode, trieNode, editDistance));
	  }

	  // go through the children
	  for (unsigned childIterator = 0; childIterator < trieNode->getChildrenCount(); childIterator ++)
	    _appendLeafNodes(prefixNode, trieNode->getChild(childIterator), editDistance, visitedTrieNodes);
	}
};

#endif //__ACTIVENODE_H__
