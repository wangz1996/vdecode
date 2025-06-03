void dfit(){
    auto f = TFile::Open("hist.root","READ");
    TH1D *h = (TH1D*)f->Get("hmh_24");
    float r1=3200;
    float r2=30000;
    h->GetXaxis()->SetRangeUser(r1,r2);
    TF1 *f1=new TF1("f1","[0]*TMath::Landau(x,[1],[2])",r1,r2);
    f1->SetParameters(1000,5000,1000);
    h->Fit("f1","","",r1,r2);
    // TH1D *h1=new TH1D("h1","test",100,0,100);
    // TRandom3 rnd(1234);
    // for(int i=0;i<100000;i++){
    //     h1->Fill(rnd.Exp(50));
    // }
    // for(int i=0;i<5000;i++){
    //     h1->Fill(rnd.Gaus(50,5));
    // }
    // TF1 *fg=new TF1("fg","[0]*TMath::Gaus(x,[1],[2])+[3]*TMath::Exp([4]*x)",0,100);
    // fg->SetParameters(100,50,5,100,-1);
    // fg->SetParLimits(1,45,55);
    // fg->SetParLimits(2,0,20);
    // TF1 *f1=new TF1("f1","[0]*TMath::Gaus(x,[1],[2])",0,100);
    // f1->SetParameters(fg->GetParameter(0),fg->GetParameter(1),fg->GetParameter(2));
    // h1->Draw();
    // h1->Fit("fg","","",0,100);
    // f1->Draw("");

}
