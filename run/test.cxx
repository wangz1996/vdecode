// float GetFitHigh(TH1D* h,std::string fname){
// 	TF1 *f1=new TF1("f1","TMath::Gaus(x,[0],[1])",1200,2500);
// 	f1->SetParameters(1700,400);
// 	h->Fit(f1,"Q","",1200,2500);
// 	auto mean = f1->GetParameter(0);
// 	std::cout<< fname<<" "<<mean/5.614*1e-15/(1.6e-19)/50./36.<<std::endl;
// 	return mean;
// }
// float GetFitLow(TH1D* h,std::string fname){
// 	TF1 *f1=new TF1("f1","TMath::Gaus(x,[0],[1])",1200,2500);
// 	f1->SetParameters(180,50);
// 	h->Fit(f1,"Q","",100,220);
// 	auto mean = f1->GetParameter(0);
// 	std::cout<< fname<<" "<<mean/0.549*1e-15/(1.6e-19)/50./36.<<std::endl;
// 	return mean;
// }
void test(const std::string& fname){
	ROOT::RDataFrame df("tree",fname);
	std::unordered_map<int,TH1D*> umap_hmh;
	std::unordered_map<int,TH1D*> umap_hml;
	std::unordered_map<int,TH1D*> umap_hbh;
	std::unordered_map<int,TH1D*> umap_hbl;
	const int N=25;
	for(int i=0;i<N;i++){
		umap_hmh[i]=new TH1D(Form("hmh_%d",i),Form("hmh_%d",i),1000,0,10000);
		umap_hml[i]=new TH1D(Form("hml_%d",i),Form("hml_%d",i),1000,0,10000);
		umap_hbh[i]=new TH1D(Form("hbh_%d",i),Form("hbh_%d",i),1000,0,10000);
		umap_hbl[i]=new TH1D(Form("hbl_%d",i),Form("hbl_%d",i),1000,0,10000);
	}
	df.Foreach([&umap_hmh,&umap_hml,&umap_hbh,&umap_hbl](const std::vector<int>& id,const std::vector<int>& adc,const std::vector<int>& plat){
			for(int i=0;i<id.size();i++){
				int cellid = id.at(i);
				int celladc = adc.at(i);
				int cellplat = plat.at(i);
				int cryid = cellid/100000-1;
				int mbid = (cellid%10000)/1000;
				//std::cout<<cryid<<std::endl;
				if((cellid%1000)/100==1){
					if(mbid==1){
						umap_hmh[cryid]->Fill(celladc-cellplat);
					}
					else{
						umap_hbh[cryid]->Fill(celladc-cellplat);
					}
				}
				else{
					if(mbid==1){
						umap_hml[cryid]->Fill(celladc-cellplat);
					}
					else{
						umap_hbl[cryid]->Fill(celladc-cellplat);
					}
				}
			}
			},{"CellID","CellADC","CellPLAT"});
	std::cout<<fname<<std::endl;
	TCanvas *cmh=new TCanvas("cmh","MH",1500,1500);
	cmh->Divide(5,5);
	for(int i=0;i<N;i++){
		cmh->cd(i+1);
		umap_hmh[i]->Draw();
	}
	TCanvas *cml=new TCanvas("cml","ML",1500,1500);
	cml->Divide(5,5);
	for(int i=0;i<N;i++){
		cml->cd(i+1);
		umap_hml[i]->Draw();
	}
	TCanvas *cbh=new TCanvas("cbh","BH",1500,1500);
	cbh->Divide(5,5);
	for(int i=0;i<N;i++){
		cbh->cd(i+1);
		umap_hbh[i]->Draw();
	}
	TCanvas *cbl=new TCanvas("cbl","BL",1500,1500);
	cbl->Divide(5,5);
	for(int i=0;i<N;i++){
		cbl->cd(i+1);
		umap_hbl[i]->Draw();
	}
	cmh->SaveAs(Form("./plot/%s_mh.pdf",fname.c_str()));
	cml->SaveAs(Form("./plot/%s_ml.pdf",fname.c_str()));
	cbh->SaveAs(Form("./plot/%s_bh.pdf",fname.c_str()));
	cbl->SaveAs(Form("./plot/%s_bl.pdf",fname.c_str()));
	TString oname=TString(fname).Replace(0,6,"hist");
	TFile *fout=new TFile(oname,"RECREATE");
	fout->cd();
	for(int i=0;i<N;i++){
		umap_hmh[i]->Write();
		umap_hml[i]->Write();
		umap_hbh[i]->Write();
		umap_hbl[i]->Write();
	}
	fout->Close();
}
