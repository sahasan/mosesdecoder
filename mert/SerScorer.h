#ifndef MERT_SER_SCORER_H_
#define MERT_SER_SCORER_H_

#include <set>
#include <string>
#include <vector>
#include "Types.h"
#include "StatisticsBasedScorer.h"

namespace MosesTuning
{


class ScoreStats;

/**
 * An implementation of sentence error rate.
 *
 * MERT maximizes, so this will return 1-SER, i.e. Accuracy:
 * acc = #correct/#total (correct here means exact matches on sentence level)
 */
class SerScorer: public StatisticsBasedScorer
{
public:
  explicit SerScorer(const std::string& config = "");
  ~SerScorer();

  virtual void setReferenceFiles(const std::vector<std::string>& referenceFiles);
  virtual void prepareStats(std::size_t sid, const std::string& text, ScoreStats& entry);
  virtual std::size_t NumberOfScores() const {
    return 3;
  }
  virtual float calculateScore(const std::vector<int>& comps) const;

private:
  // no copying allowed
  SerScorer(const SerScorer&);
  SerScorer& operator=(const SerScorer&);

  // data extracted from reference files
  std::vector<std::size_t> m_ref_lengths;
  std::vector<std::vector<int> > m_ref_tokens;
};

}

#endif  // MERT_SER_SCORER_H_
