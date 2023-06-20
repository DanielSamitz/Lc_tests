void combine_histos(TString in_file_name1, TString in_file_name2, TString out_name = "AnalysisResults.root"){
   TFile * file1 = TFile::Open(in_file_name1);
   TFile * file2 = TFile::Open(in_file_name2);
   TFile *outputFile = new TFile(out_name,"recreate");

   TIter next(file1->GetListOfKeys());
   TKey* key = nullptr;
   while ((key = static_cast<TKey*>(next()))) {
      auto obj= key->ReadObj();
      if (auto nextInDir = dynamic_cast<TDirectory*>(obj)){
         TDirectory *outdir = outputFile->mkdir(obj->GetName());
         outdir->cd();
         TIter next2(nextInDir->GetListOfKeys());
         TKey* key2 = nullptr;
         while ((key2 = static_cast<TKey*>(next2()))) {
            auto obj2 = key2->ReadObj();
            if (auto hist = dynamic_cast<TH3*>(obj2)){
               printf("In dir %s found TH3 %s\n",obj->GetName(),obj2->GetName());
               TH3F* hist2 = new TH3F();
               file2->GetObject(obj->GetName()+TString("/")+obj2->GetName(),hist2);
               hist->Add(hist2);
               hist->Write();
            }
            else if (auto hist = dynamic_cast<TH2*>(obj2)){
               printf("In dir %s found TH2 %s\n",obj->GetName(),obj2->GetName());
               TH2F* hist2 = new TH2F();
               file2->GetObject(obj->GetName()+TString("/")+obj2->GetName(),hist2);
               hist->Add(hist2);
               hist->Write();
            }
            else if (auto hist = dynamic_cast<TH1*>(obj2)){
               printf("In dir %s found TH1 %s\n",obj->GetName(),obj2->GetName());
               TH1F* hist2 = new TH1F();
               file2->GetObject(obj->GetName()+TString("/")+obj2->GetName(),hist2);
               hist->Add(hist2);
               hist->Write();
            }
         }
         outdir->Write();
      }
   }
}