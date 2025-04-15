void draw_ped(std::string fname){
    ROOT::RDataFrame df("tree",fname);
    TH1D *hpmh = new TH1D("hpmh","hplat",100,4000,10000);
    TH1D *hpbh = new TH1D("hpbh","hplat",100,4000,10000);
    TH1D *hpml = new TH1D("hpml","hplat",100,4000,10000);
    TH1D *hpbl = new TH1D("hpbl","hplat",100,4000,10000);
    const int n_bins = 1000;
    double bins[n_bins + 1]; // 存储 bin 边界
    double log_min = 0; // 最小值的对数
    double log_max = 7;
    double log_bin_width = (log_max - log_min) / n_bins; // 对数 bin 宽度
    for (int i = 0; i <= n_bins; i++) {
        bins[i] = TMath::Power(10, log_min + i * log_bin_width);
    }
    TH1D *hmh = new TH1D("hmh","hE",n_bins,bins);
    TH1D *hml = new TH1D("hml","hE",n_bins,bins);
    TH1D *hbh = new TH1D("hbh","hE",n_bins,bins);
    TH1D *hbl = new TH1D("hbl","hE",n_bins,bins);
    TH1D *hh = new TH1D("hh","hE",n_bins,bins);
    TH1D *hl = new TH1D("hl","hE",n_bins,bins);
    std::unordered_map<int,TH1D*> umap_hpmh;
	std::unordered_map<int,TH1D*> umap_hpml;
	std::unordered_map<int,TH1D*> umap_hpbh;
	std::unordered_map<int,TH1D*> umap_hpbl;
    std::unordered_map<int,TH1D*> umap_hmh;
	std::unordered_map<int,TH1D*> umap_hml;
	std::unordered_map<int,TH1D*> umap_hbh;
	std::unordered_map<int,TH1D*> umap_hbl;
	const int N=25;
    
	for(int i=0;i<N;i++){
		umap_hpmh[i]=new TH1D(Form("hpmh_%d",i),Form("hmh_%d",i),1000,4000,10000);
		umap_hpml[i]=new TH1D(Form("hpml_%d",i),Form("hml_%d",i),1000,4000,10000);
		umap_hpbh[i]=new TH1D(Form("hpbh_%d",i),Form("hbh_%d",i),1000,4000,10000);
		umap_hpbl[i]=new TH1D(Form("hpbl_%d",i),Form("hbl_%d",i),1000,4000,10000);
		umap_hmh[i]=new TH1D(Form("hmh_%d",i),Form("hmh_%d",i),n_bins,bins);
		umap_hml[i]=new TH1D(Form("hml_%d",i),Form("hml_%d",i),n_bins,bins);
		umap_hbh[i]=new TH1D(Form("hbh_%d",i),Form("hbh_%d",i),n_bins,bins);
		umap_hbl[i]=new TH1D(Form("hbl_%d",i),Form("hbl_%d",i),n_bins,bins);
	}
    df.Foreach([hpmh,hpbh,hpml,hpbl,&umap_hpmh,&umap_hpml,&umap_hpbh,&umap_hpbl,&hmh,&hml,&hbh,&hbl,&hh,&hl,&umap_hmh,&umap_hml,&umap_hbh,&umap_hbl](const std::vector<int>& id,const std::vector<int>& adc,const std::vector<int>& plat){
        float mh=0.;
        float ml=0.;
        float bh=0.;
        float bl=0.;
        float high=0.;
        float low=0.;
			for(int i=0;i<id.size();i++){
				int cellid = id.at(i);
				int celladc = adc.at(i);
				int cellplat = plat.at(i);
				int cryid = cellid/100000;
				int mbid = (cellid%10000)/1000;
				if(cryid==0){
					std::cout<<cellid<<" "<<cryid<<std::endl;
					continue;
				}
				if((cellid%1000)/100==1){ // High Gain
					if(mbid==1){
                        mh+=(celladc-cellplat);
                        hpmh->Fill(cellplat);
                        umap_hpmh[cryid-1]->Fill(cellplat);
		    	umap_hmh[cryid-1]->Fill(celladc-cellplat);
					}
					else{
                        bh+=(celladc-cellplat);
                        hpbh->Fill(cellplat);
                        umap_hpbh[cryid-1]->Fill(cellplat);
		    	umap_hbh[cryid-1]->Fill(celladc-cellplat);
					}
                    high += (celladc-cellplat);
				}
				else{ // Low Gain
					if(mbid==1){
                        ml+=(celladc-cellplat);
                        hpml->Fill(cellplat);
                        umap_hpml[cryid-1]->Fill(cellplat);
		    	umap_hml[cryid-1]->Fill(celladc-cellplat);
					}
					else{
                        bl+=(celladc-cellplat);
                        hpbl->Fill(cellplat);
                        umap_hpbl[cryid-1]->Fill(cellplat);
		    	umap_hbl[cryid-1]->Fill(celladc-cellplat);
					}
                    low += (celladc-cellplat);
				}
			}
            hmh->Fill(mh);
            hml->Fill(ml);
            hbh->Fill(bh);
            hbl->Fill(bl);
            hh->Fill(high);
            hl->Fill(low);
			},{"CellID","CellADC","CellPLAT"});
    std::string oname = fname;
    oname = oname.substr(0,oname.find_last_of("."));
    oname = oname.substr(oname.find_first_of('_')+1);
    auto fout = new TFile(TString::Format("%s.root",oname.c_str()),"RECREATE");
    fout->cd();
    hpmh->Write();
    hpbh->Write();
    hpml->Write();
    hpbl->Write();
    hmh->Write();
    hml->Write();
    hbh->Write();
    hbl->Write();
    hh->Write();
    hl->Write();
    for(int i=0;i<N;i++){
        umap_hpmh[i]->Write();
        umap_hpml[i]->Write();
        umap_hpbh[i]->Write();
        umap_hpbl[i]->Write();
        umap_hmh[i]->Write();
        umap_hml[i]->Write();
        umap_hbh[i]->Write();
        umap_hbl[i]->Write();
    }
    fout->Close();

}

void draw(std::string fname){
    draw_ped(fname);
}
