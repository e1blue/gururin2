package main

import(
	"os"
	"log"
	"fmt"
	"time"
	"net/http"
	"html/template"
	"appengine"
	"appengine/datastore"
	"encoding/json"
	"io/ioutil"
	"strings"
	"strconv"
	"regexp"
);

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 編集ステージ保存データ TODO 未使用
type EditedData struct{
	Name string
	EditData []byte
	Date time.Time
};

// リプレイ情報保存データ
type ReplayData struct{
	Platform string
	Version string
	UserId string
	StageId int
	StageIndex int
	PuppetId int
	Score int
	Damage int
	Seed []int
	GravityList []byte
	GravityLength int
	Date time.Time
};

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 編集ステージ確認入力構造体
type EditedShowRequestBody struct{
	EditedData string `json:"edited"`
	EditorData string `json:"editor"`
};

// リプレイ情報保存入力構造体
type ReplaySaveRequestBody struct{
	Platform string `json:"platform"`
	Version string `json:"version"`
	UserId string `json:"user_id"`
	StageId int `json:"stage_id"`
	StageIndex int `json:"stage_index"`
	PuppetId int `json:"puppet_id"`
	Score int `json:"score"`
	Damage int `json:"damage"`
	Seed []string `json:"seed"`
	GravityList string `json:"gravityList"`
	GravityLength int `json:"gravityLength"`
};

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// テンプレートをキャッシュ
var templates = make(map[string]*template.Template);

// テンプレート用関数 日本時間表記
func tempFuncLocalTime(input time.Time) string{
	loc, _ := time.LoadLocation("Asia/Tokyo");
	return input.In(loc).Format("2006/01/02 15:04:05");
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// basic認証の確認
func checkAuth(res http.ResponseWriter, req *http.Request) bool{
	username, password, ok := req.BasicAuth();

	if ok && username == "user" && password == "pass" {return true;}

	res.Header().Set("WWW-Authenticate", "Basic realm=\"Authorization Required\"")
	http.Error(res, "Unauthorized", http.StatusUnauthorized);
	return false;
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 初期化
func init(){
	http.HandleFunc("/test", testHandler);
	http.HandleFunc("/landing", landingHandler);
	http.HandleFunc("/play", playHandler);
	http.HandleFunc("/editor", editorHandler);
	http.HandleFunc("/edited", editedHandler);
	http.HandleFunc("/replay/", replayHandler);
	http.HandleFunc("/replay_save", replaySaveHandler);
	http.HandleFunc("/replay_list", replayListHandler);
	http.HandleFunc("/admin", adminHandler);
	http.HandleFunc("/", staticHandler);

	// テンプレートをキャッシュ
	funcMap := template.FuncMap{
		"localTime": tempFuncLocalTime,
	};
	templates["landing"] = template.Must(template.New("layout.html").Funcs(funcMap).ParseFiles("views/layout.html", "views/landing.html"));
	templates["play"] = template.Must(template.New("layout.html").Funcs(funcMap).ParseFiles("views/layout.html", "views/play.html"));
	templates["editor"] = template.Must(template.New("layout.html").Funcs(funcMap).ParseFiles("views/layout.html", "views/editor.html"));
	templates["edited"] = template.Must(template.New("layout.html").Funcs(funcMap).ParseFiles("views/layout.html", "views/edited.html"));
	templates["replay"] = template.Must(template.New("layout.html").Funcs(funcMap).ParseFiles("views/layout.html", "views/replay.html"));
	templates["replayList"] = template.Must(template.New("layout.html").Funcs(funcMap).ParseFiles("views/layout.html", "views/replayList.html"));
	templates["admin"] = template.Must(template.New("layout.html").Funcs(funcMap).ParseFiles("views/layout.html", "views/admin.html"));
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ほげ
func testHandler(res http.ResponseWriter, req *http.Request){
	if !checkAuth(res, req) {return;}

	fmt.Fprint(res, "Hello, world!");
	log.Printf("Hello logging");
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ランディングページ
func landingHandler(res http.ResponseWriter, req *http.Request){
	//if !checkAuth(res, req) {return;}

	// html描画
	err := templates["landing"].Execute(res, map[string]interface{}{
		"Title":  "landing",
	});
	if err != nil {http.Error(res, err.Error(), http.StatusInternalServerError); return;}
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// プレイ開始
func playHandler(res http.ResponseWriter, req *http.Request){
	//if !checkAuth(res, req) {return;}

	// html描画
	err := templates["play"].Execute(res, map[string]interface{}{
		"Title":  "play",
	});
	if err != nil {http.Error(res, err.Error(), http.StatusInternalServerError); return;}
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ステージエディタ起動
func editorHandler(res http.ResponseWriter, req *http.Request){
	if !checkAuth(res, req) {return;}

	var reqData EditedShowRequestBody;
	if req.Method == "POST" {
		req.ParseForm();
		if req.Form["edited"] == nil {http.NotFound(res, req); return;}
		if req.Form["editor"] == nil {http.NotFound(res, req); return;}
		reqData = EditedShowRequestBody{
			EditedData: req.Form["edited"][0],
			EditorData: req.Form["editor"][0],
		};
	}else{
		reqData = EditedShowRequestBody{
			EditedData: "{\"map\":[[[1,1,1,1],[1,0,0,1],[1,1,1,1]]],\"player\":[{\"x\":1,\"y\":1,\"hp\":100}],\"enemy\":[],\"item\":[{\"type\":\"star\",\"x\":2,\"y\":1}]}",
			EditorData: "",
		};
	}

	// html描画
	err := templates["editor"].Execute(res, map[string]interface{}{
		"Title":  "editor",
		"EditedData": reqData.EditedData,
		"EditorData": reqData.EditorData,
	});
	if err != nil {http.Error(res, err.Error(), http.StatusInternalServerError); return;}
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 編集ステージプレイ開始
func editedHandler(res http.ResponseWriter, req *http.Request){
	if !checkAuth(res, req) {return;}
	if req.Method != "POST" {http.NotFound(res, req); return;}

	req.ParseForm();
	if req.Form["edited"] == nil {http.NotFound(res, req); return;}
	if req.Form["editor"] == nil {http.NotFound(res, req); return;}
	reqData := EditedShowRequestBody{
		EditedData: req.Form["edited"][0],
		EditorData: req.Form["editor"][0],
	};

	// html描画
	err := templates["edited"].Execute(res, map[string]interface{}{
		"Title":  "edited",
		"EditedData": reqData.EditedData,
		"EditorData": reqData.EditorData,
	});
	if err != nil {http.Error(res, err.Error(), http.StatusInternalServerError); return;}
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// リプレイ再生
func replayHandler(res http.ResponseWriter, req *http.Request){
	//if !checkAuth(res, req) {return;}

	// ID形式確認
	replayStr := req.URL.Path[len("/replay/"):];
	replayValidator := regexp.MustCompile("^[0-9]+$");
	if !replayValidator.MatchString(replayStr) {http.NotFound(res, req); return;}
	replayId, err := strconv.ParseInt(replayStr, 10, 64);
	if err != nil {http.Error(res, err.Error(), http.StatusInternalServerError); return;}

	// データ読み込み
	var data ReplayData;
	context := appengine.NewContext(req);
	key := datastore.NewKey(context, "replaydatas", "", replayId, nil);
	err = datastore.Get(context, key, &data);
	if err != nil && err == datastore.ErrNoSuchEntity {http.NotFound(res, req); return;}
	if err != nil {http.Error(res, err.Error(), http.StatusInternalServerError); return;}

	// html描画
	err = templates["replay"].Execute(res, map[string]interface{}{
		"Title":  "replay",
		"ReplayData": data,
		"GravityList": string(data.GravityList),
	});
	if err != nil {http.Error(res, err.Error(), http.StatusInternalServerError); return;}
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// リプレイ情報保存
func replaySaveHandler(res http.ResponseWriter, req *http.Request){
	if req.Method != "POST" {http.NotFound(res, req); return;}

	// jsonデコード
	var reqData ReplaySaveRequestBody;
	body, err := ioutil.ReadAll(req.Body);
	if err != nil {http.Error(res, err.Error(), http.StatusInternalServerError); return;}
	err = json.Unmarshal(body, &reqData);
	if err != nil {http.Error(res, err.Error(), http.StatusInternalServerError); return;}

	// データ書き込み
	data := ReplayData{
		Platform: reqData.Platform,
		Version: reqData.Version,
		UserId: reqData.UserId,
		StageId: reqData.StageId,
		StageIndex: reqData.StageIndex,
		PuppetId: reqData.PuppetId,
		Score: reqData.Score,
		Damage: reqData.Damage,
		GravityList: []byte(reqData.GravityList),
		GravityLength: reqData.GravityLength,
		Date: time.Now(),
	};
	data.Seed = make([]int, 4);
	data.Seed[0], _ = strconv.Atoi(reqData.Seed[0]);
	data.Seed[1], _ = strconv.Atoi(reqData.Seed[1]);
	data.Seed[2], _ = strconv.Atoi(reqData.Seed[2]);
	data.Seed[3], _ = strconv.Atoi(reqData.Seed[3]);
	context := appengine.NewContext(req);
	key := datastore.NewIncompleteKey(context, "replaydatas", nil);
	key, err = datastore.Put(context, key, &data);
	if err != nil {http.Error(res, err.Error(), http.StatusInternalServerError); return;}

	// jsonエンコードと出力
	bytes, err := json.Marshal(map[string]interface{}{
		"key": strconv.FormatInt(key.IntID(), 10),
	});
	if err != nil {http.Error(res, err.Error(), http.StatusInternalServerError); return;}
	fmt.Fprint(res, string(bytes));
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// リプレイ情報確認
func replayListHandler(res http.ResponseWriter, req *http.Request){
	if !checkAuth(res, req) {return;}

	// データ読み込み
	context := appengine.NewContext(req);
	query := datastore.NewQuery("replaydatas").Order("-Date").Limit(10);
	datas := make([]interface{}, 0);
	iter := query.Run(context);
	for{
		var data ReplayData;
		key, err := iter.Next(&data);
		if err == datastore.Done {break;}
		if err != nil {http.Error(res, err.Error(), http.StatusInternalServerError); return;}
		datas = append(datas, map[string]interface{}{
			"Id": key.IntID(),
			"Data": data,
		});
	}

	// html描画
	err := templates["replayList"].Execute(res, map[string]interface{}{
		"Title": "replay list",
		"ReplayDatas": datas,
	});
	if err != nil {http.Error(res, err.Error(), http.StatusInternalServerError); return;}
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 管理者ツール
func adminHandler(res http.ResponseWriter, req *http.Request){
	if !checkAuth(res, req) {return;}

	// html描画
	err := templates["admin"].Execute(res, map[string]interface{}{
		"Title":  "admin",
	});
	if err != nil {http.Error(res, err.Error(), http.StatusInternalServerError); return;}
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ディレクトリ構造を見せないための構造体
type FuhahaDir struct{}
func (dir FuhahaDir) Open(name string) (http.File, error){
	const staticDir = "./statics";
	const indexPage = "/index.html";
	file, err := os.Open(staticDir + name);
	if err != nil && strings.HasSuffix(name, indexPage) {return os.Open(staticDir + indexPage);}
	if err != nil {return nil, err;}
	return file, nil;
}

// 静的ファイル配信 認証処理を加えるためにgo言語で処理
func staticHandler(res http.ResponseWriter, req *http.Request){
	//if !checkAuth(res, req) {return;}

	if req.URL.Path == "/" {
		// ランディングページを表示
		landingHandler(res, req);
	}else{
		req.Header.Del("If-Modified-Since")
		handler := http.FileServer(FuhahaDir{});
		handler.ServeHTTP(res, req);
	}
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

