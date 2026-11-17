# C++ 3D Software Renderer (From Scratch)

OpenGL や DirectX などの外部グラフィックスライブラリを一切使用せず、C++ 標準ライブラリのみでゼロから構築した3Dソフトウェアレンダラ（ラスタライザ）です。

> **【参考元・ベース教材 (Credits & Sources)】**
> 本プロジェクトのレンダリングパイプライン設計および理論学習は、以下のオープンソース教材・ドキュメントをベースとしています[cite: 5]：
> * **tinyrenderer** by Dmitry V. Sokolov (`ssloy/tinyrenderer`) — [GitHub](https://github.com/ssloy/tinyrenderer) / [Course Notes](https://haqr.eu/tinyrenderer/)[cite: 2]
> * **Scratchapixel** — [Website](https://www.scratchapixel.com/)[cite: 3]
> 
> ※ 補助ファイル（`tgaimage.h/.cpp`, `model.h/.cpp`, `geometry.h`）およびテスト用3Dモデル（`african_head.obj`）は `ssloy/tinyrenderer`（Zlib/PNG License）のものを利用し[cite: 2, 7]、コアとなるラスタライズ・座標変換・シェーディング等のアルゴリズムは理論をもとに自力で実装しています[cite: 5, 9, 10]。

---

## 開発の目的 (Overview)

現代のGPUレンダリングパイプライン（頂点処理・ラスタライズ・フラグメント処理）の内部構造、低レイヤーのメモリ管理、および3Dグラフィックスの数学的基礎を深く理解するために開発しています[cite: 8, 13]。

## 主な実装機能 (Current Features)

* **フレームバッファ & 画像出力:** 1次元バイト配列による画面メモリ管理および TGA フォーマットでの画像出力[cite: 8]
* **ラスタライザ:** バウンディングボックス最適化と重心座標系（Barycentric Coordinates）を用いた三角形の塗りつぶし[cite: 6]
* **隠面消去 (Z-Buffer) & カリング:** 深度バッファによる前後判定と、外積を用いたバックフェースカリングによる高速化[cite: 6, 10]
* **カメラ & 透視投影:** `lookat` 変換行列および斉次座標によるパースペクティブ投影（MVP変換パイプラインの基礎）[cite: 9]
* **シェーディング:** 法線ベクトルと光源の内積を用いたフラット／スムース（Gouraud / Phong）シェーディング[cite: 6, 10]

## 今後の実装予定 (Roadmap / Original Extensions)

基礎パイプラインの構築後、低スペック環境（CPU）でのリアルタイム描画・高速化に向けて以下の独自拡張を実装予定です[cite: 4]。

* **リアルタイムウィンドウ表示 & インタラクティブ操作:** Win32 API を用いたフレームバッファの直接転送とカメラ移動[cite: 4]
* **確率的スパース・レンダリング & テンポラル再投影:** 全ピクセルの計算を間引き、前フレームの逆行列再投影と拡散方程式ベースの空間フィルタで補間・復元する軽量描画パイプラインの検証[cite: 4]

## ビルドと実行環境 (Build & Environment)

* **言語:** C++17[cite: 8]
* **ビルドシステム:** CMake (3.10+) / Visual Studio 2022[cite: 8]

```bash
cmake -B build
cmake --build build --config Release
