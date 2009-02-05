/***********************************************************************
Moses - factored phrase-based language decoder
Copyright (C) 2009 University of Edinburgh

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
***********************************************************************/

#include "Decoder.h"

using namespace std;
using namespace Moses;


namespace Josiah {

  /**
    * Allocates a char* and copies string into it.
  **/
  static char* strToChar(const string& s) {
    char* c = new char[s.size()+1];
    strcpy(c,s.c_str());
    return c;
  }
    
  
  void initMoses(const string& inifile, int debuglevel, int argc, char** argv) {
    static int BASE_ARGC = 4;
    Parameter* params = new Parameter();
    char ** mosesargv = new char*[BASE_ARGC + argc];
    mosesargv[0] = "-f";
    mosesargv[1] = strToChar(inifile);
    mosesargv[2] = "-v";
    stringstream dbgin;
    dbgin << debuglevel;
    mosesargv[3] = strToChar(dbgin.str());
    
    for (int i = 0; i < argc; ++i) {
      mosesargv[BASE_ARGC + i] = argv[i];
    }
    params->LoadParam(BASE_ARGC + argc,mosesargv);
    StaticData::LoadDataStatic(params);
    delete mosesargv[1];
    delete mosesargv[3];
    delete mosesargv;
  }
  
  MosesDecoder::MosesDecoder() : m_searcher(NULL) {}
  
  void MosesDecoder::decode(const std::string& source, Hypothesis*& bestHypo, TranslationOptionCollection*& toc) {
     
      const StaticData &staticData = StaticData::Instance();
  
      //the sentence
      Sentence sentence(Input);
      stringstream in(source + "\n");
      //in << "\n";
      cout << in.str() << endl;
      const std::vector<FactorType> &inputFactorOrder = staticData.GetInputFactorOrder();
      sentence.Read(in,inputFactorOrder);
  
      //the searcher
      staticData.ResetSentenceStats(sentence);
      staticData.InitializeBeforeSentenceProcessing(sentence);
      toc  =sentence.CreateTranslationOptionCollection();
      const vector <DecodeGraph*>
            &decodeStepVL = staticData.GetDecodeStepVL();
      toc->CreateTranslationOptions(decodeStepVL);
      
      delete m_searcher;
      m_searcher = new SearchNormal(sentence,*toc);
      m_searcher->ProcessSentence();
  
      //get hypo
      bestHypo = const_cast<Hypothesis*>(m_searcher->GetBestHypothesis());
  }

}
