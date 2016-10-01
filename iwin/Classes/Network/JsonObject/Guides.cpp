#include "Guides.h"
#include "json/writer.h"
namespace iwinmesage
{
	
	Guides:: Guides ()
	{

	}

	Guides::~Guides()
	{

	}

	rapidjson::Document Guides::toJson()
	{

		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType& locator = document.GetAllocator();
		rapidjson::Value vhelp(help.c_str(), help.size());
		document.AddMember("help", vhelp,locator);
rapidjson::Value vpayment(payment.c_str(), payment.size());
		document.AddMember("payment", vpayment,locator);
rapidjson::Value vwinfree(winfree.c_str(), winfree.size());
		document.AddMember("winfree", vwinfree,locator);
rapidjson::Value vactivity(activity.c_str(), activity.size());
		document.AddMember("activity", vactivity,locator);
rapidjson::Value vbetasia(betasia.c_str(), betasia.size());
		document.AddMember("betasia", vbetasia,locator);
rapidjson::Value vstock(stock.c_str(), stock.size());
		document.AddMember("stock", vstock,locator);
rapidjson::Value vmoonfestival(moonfestival.c_str(), moonfestival.size());
		document.AddMember("moonfestival", vmoonfestival,locator);
rapidjson::Value vindependence(independence.c_str(), independence.size());
		document.AddMember("independence", vindependence,locator);
rapidjson::Value vhalloween(halloween.c_str(), halloween.size());
		document.AddMember("halloween", vhalloween,locator);
rapidjson::Value vphucloctho(phucloctho.c_str(), phucloctho.size());
		document.AddMember("phucloctho", vphucloctho,locator);
rapidjson::Value vlixi(lixi.c_str(), lixi.size());
		document.AddMember("lixi", vlixi,locator);
rapidjson::Value vdangtrong(dangtrong.c_str(), dangtrong.size());
		document.AddMember("dangtrong", vdangtrong,locator);
rapidjson::Value vrankingdomino(rankingdomino.c_str(), rankingdomino.size());
		document.AddMember("rankingdomino", vrankingdomino,locator);
rapidjson::Value vdice(dice.c_str(), dice.size());
		document.AddMember("dice", vdice,locator);
rapidjson::Value vgiaidau(giaidau.c_str(), giaidau.size());
		document.AddMember("giaidau", vgiaidau,locator);
rapidjson::Value vgame(game.c_str(), game.size());
		document.AddMember("game", vgame,locator);

		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		return document;
	}
	void Guides::toData(std::string json)
	{
		rapidjson::Document document;
		document.Parse(json.c_str());
		toData(document);
	}
	void Guides::toData(rapidjson::Document & document)
	{

		if(document.HasMember("help"))
		{
			setHelp(document["help"].GetString());
		}
		if(document.HasMember("payment"))
		{
			setPayment(document["payment"].GetString());
		}
		if(document.HasMember("winfree"))
		{
			setWinfree(document["winfree"].GetString());
		}
		if(document.HasMember("activity"))
		{
			setActivity(document["activity"].GetString());
		}
		if(document.HasMember("betasia"))
		{
			setBetasia(document["betasia"].GetString());
		}
		if(document.HasMember("stock"))
		{
			setStock(document["stock"].GetString());
		}
		if(document.HasMember("moonfestival"))
		{
			setMoonfestival(document["moonfestival"].GetString());
		}
		if(document.HasMember("independence"))
		{
			setIndependence(document["independence"].GetString());
		}
		if(document.HasMember("halloween"))
		{
			setHalloween(document["halloween"].GetString());
		}
		if(document.HasMember("phucloctho"))
		{
			setPhucloctho(document["phucloctho"].GetString());
		}
		if(document.HasMember("lixi"))
		{
			setLixi(document["lixi"].GetString());
		}
		if(document.HasMember("dangtrong"))
		{
			setDangtrong(document["dangtrong"].GetString());
		}
		if(document.HasMember("rankingdomino"))
		{
			setRankingdomino(document["rankingdomino"].GetString());
		}
		if(document.HasMember("dice"))
		{
			setDice(document["dice"].GetString());
		}
		if(document.HasMember("giaidau"))
		{
			setGiaidau(document["giaidau"].GetString());
		}
		if(document.HasMember("game"))
		{
			setGame(document["game"].GetString());
		}

	}
}
