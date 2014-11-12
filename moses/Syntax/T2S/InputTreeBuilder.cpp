#include "InputTreeBuilder.h"

#include "moses/StaticData.h"

namespace Moses
{
namespace Syntax
{
namespace T2S
{

InputTreeBuilder::InputTreeBuilder()
  : m_outputFactorOrder(StaticData::Instance().GetOutputFactorOrder())
{
}

void InputTreeBuilder::Build(const TreeInput &in, InputTree &out)
{
  CreateNodes(in, out);
  ConnectNodes(out);
}

// Create the InputTree::Node objects but do not connect them.
void InputTreeBuilder::CreateNodes(const TreeInput &in, InputTree &out)
{
  // Get the input sentence word count.  This includes the <s> and </s> symbols.
  const std::size_t numWords = in.GetSize();

  // Get the parse tree non-terminal nodes.  This does not include the top-level
  // node that covers <s> and </s>.
  const std::vector<XMLParseOutput> &xmlNodes = in.GetNonTerminalsPostOrder();

  // Copy the parse tree non-terminal nodes, but offset the ranges by 1 (to
  // allow for the <s> symbol at position 0) and add a top-level node.
  std::vector<XMLParseOutput> nonTerms;
  nonTerms.reserve(xmlNodes.size()+1);
  for (std::vector<XMLParseOutput>::const_iterator p = xmlNodes.begin();
       p != xmlNodes.end(); ++p) {
    std::size_t start = p->m_range.GetStartPos();
    std::size_t end = p->m_range.GetEndPos();
    nonTerms.push_back(XMLParseOutput(p->m_label, WordsRange(start+1, end+1)));
  }
  // FIXME Label won't always be "Q"!
  nonTerms.push_back(XMLParseOutput("Q", WordsRange(0, numWords-1)));

  // Allocate space for the InputTree nodes.  In the case of out.nodes, this
  // step is essential because once created the PVertex objects must not be
  // moved around (through vector resizing) because InputTree keeps pointers
  // to them.
  out.nodes.reserve(numWords + nonTerms.size());
  out.nodesAtPos.resize(numWords);

  // First create the InputTree::Node objects but do not connect them.
  int prevStart = -1;
  int prevEnd = -1;
  for (std::vector<XMLParseOutput>::const_iterator p = nonTerms.begin();
       p != nonTerms.end(); ++p) {
    int start = static_cast<int>(p->m_range.GetStartPos());
    int end = static_cast<int>(p->m_range.GetEndPos());

    // Check if we've started ascending a new subtree.
    if (start != prevStart && end != prevEnd) {
      // Add a node for each terminal to the left of or below the first
      // nonTerm child of the subtree.
      for (int i = prevEnd+1; i <= end; ++i) {
        PVertex v(WordsRange(i, i), in.GetWord(i));
        out.nodes.push_back(InputTree::Node(v));
        out.nodesAtPos[i].push_back(&out.nodes.back());
      }
    }
    // Add a node for the non-terminal.
    Word w(true);
    w.CreateFromString(Moses::Output, m_outputFactorOrder, p->m_label, true);
    PVertex v(WordsRange(start, end), w);
    out.nodes.push_back(InputTree::Node(v));
    out.nodesAtPos[start].push_back(&out.nodes.back());

    prevStart = start;
    prevEnd = end;
  }
}

// Connect the nodes by filling in the node.children vectors.
void InputTreeBuilder::ConnectNodes(InputTree &out)
{
  // Create a vector that records the parent of each node (except the root).
  std::vector<InputTree::Node*> parents(out.nodes.size(), NULL);
  for (std::size_t i = 0; i < out.nodes.size()-1; ++i) {
    const InputTree::Node &node = out.nodes[i];
    std::size_t start = node.pvertex.span.GetStartPos();
    std::size_t end = node.pvertex.span.GetEndPos();
    // Find the next node (in post-order) that completely covers node's span.
    std::size_t j = i+1;
    while (true) {
      const InputTree::Node &succ = out.nodes[j];
      std::size_t succStart = succ.pvertex.span.GetStartPos();
      std::size_t succEnd = succ.pvertex.span.GetEndPos();
      if (succStart <= start && succEnd >= end) {
        break;
      }
      ++j;
    }
    parents[i] = &(out.nodes[j]);
  }

  // Add each node to its parent's list of children (except the root).
  for (std::size_t i = 0; i < out.nodes.size()-1; ++i) {
    InputTree::Node &child = out.nodes[i];
    InputTree::Node &parent = *(parents[i]);
    parent.children.push_back(&child);
  }
}

}  // T2S
}  // Syntax
}  // Moses
