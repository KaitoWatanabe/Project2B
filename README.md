Project2B
=========

##導入方法
1. **VS 2013**を開く
2. **ソース管理から開く...**をクリック → 右側に**チーム エクスプローラー - ホーム**が表示される
3. **チームプロジェクトへの接続（コンセントのマーク）**をクリック
4. **ローカルGitリポジトリ** のところの**複製**をクリック
5. **複製するGitリポジトリのURLを入力してください<必須>**のところに  
https://github.com/KaitoWatanabe/Project2B.git  を入力
6. 保存先のディレクトリを指定する
7. **複製**をクリック
8. 複製されたリポジトリ(**Project2B**)をダブルクリック
9. **ソリューション**の**Project2B.sln**をダブルクリック
10. **ソリューションエクスプローラーのタブ**をクリックすればプロジェクトが表示される

##ソリューションを開く
1. **VS 2013**を開く
2. **ソース管理から開く...**をクリック → 右側に**チーム エクスプローラー - ホーム**が表示される
3. **Project2B.sln**をダブルクリック
4. **ソリューションエクスプローラーのタブ**をクリック

========================
###以降の操作は必ず**Project2B.sln**を開いて行う

##新規プロジェクトの作成
1. **ソリューションエクスプローラー**の**ソリューション'Project2B'**を右クリック
2. **追加**>**新しいプロジェクト**
3. 名前を付けて、**OK**をクリック

##新規ファイルの作成
1. プロジェクト名を右クリック
2. **追加**>**新しい項目**
3. .cpp または .h ファイルを追加する

##変更をgitに反映させる
1. **ソリューション'Project2B'**を右クリック
2. **コミット**をクリック
3. **コミット メッセージを入力してください <必須>** のところに変更内容を書く  
例：add test2
4. **コミット**をクリック
5. **変更をサーバーと共有するには、同期をクリックしてください** の**同期**をクリック
6. **同期**をクリック

##自分の以外のプロジェクト(ソースファイルなど)を編集する
1. **チームエクスプローラーのタブ**をクリック
2. **ホームボタン(家のマーク)**をクリック
3. **分岐**をクリック
4. **新しい分岐**をクリック
5. **分岐名を入力してください<必須>** のところに  
**dev-プロジェクト名**  
と入力
6. **分岐の作成**をクリック
7. その後、プロジェクト(ソースファイルなど)を編集する  
＊自分のプロジェクトを編集する際は分岐が**master**になっていることを確認する
