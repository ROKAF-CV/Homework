Introduction
---
K-means Clustering 는 주어진 데이터를 비슷한 데이터끼리 군집을 형성하는 것이다.영상처리에서 k-means를 이용하여 영역을 분할할 수 있다. 
이미지의 화소 전체를 RGB 3차원 공간으로 나타냈을때 색깔이 비슷할수록 가까이 위치할 것이다. 색깔이 비슷한 화소끼리 모아 같은 집단이라고 하면 영역 분할을 할 수 있다.

---

Algorithm
---
K-means 전반적인 알고리즘은 다음과 같다. 
[위키피디아](https://en.wikipedia.org/wiki/K-means_clustering)를 참고하면 알고리즘을 자세하게 알 수 있다.

* 입력: 컬러 영상, cluster 개수 k, 최대 반복횟수 iter
* 출력: 군집화된 영상

1. 임의로 군집의 중심점 k개를 선택한다.
<br/>
2. 모든 화소에 대해 군집의 중심점과의 거리를 측정한다.
<br/>
3. 거리가 가장 가까운 중심점이 속한 군집으로 해당 화소를 배정한다.
<br/>
4. 군집 내의 화소들의 평균으로 군집의 중심점을 업데이트한다.  
<br/>
5. 군집의 중심점의 변화가 있거나 반복횟수 iter를 넘지 않았으면 2-4를 반복한다.  
<br/>
6. 영상에서 나타난 모든 화소에 자신이 속한 군집의 번호를 매기고 같은 번호이면 같은 색을 부여한다.  

<br/>
<br/>

군집의 개수 k를 사용자가 미리 지정해주면 k개의 군집의 중심점 초기화가 필요하다.
여기에서는 k개 군집의 중심점을 영상의 화소에서 임의로 선택하는 Forgy 알고리즘을 사용한다. 이외에도 Kaufman, Macqueen 등 다양한 방식이 있으니 위키를 참고하면 된다. 해당코드는 다음과 같다. 영상의 모든 화소집합인 v에서 임의의 군집 중심점을 선정하기 위해 'mersenne twister'라는 PRNG를 사용하였다.

~~~C++
vector<Vec3b> Kmeans::init_cluster(vector<Vec3b>v,int k,int n) {
	//임의의 군집 중심 선정
	std::random_device rd;
	std::mt19937 mersenne(rd());
	std::uniform_int_distribution<> randomNum(0, n);

	vector<Vec3b> cluster;
	for (int i = 0; i < k; i++) {
		int idx = randomNum(mersenne);
		cluster.push_back(v[idx]);
	}
	return cluster;
}
~~~

<br/>
<br/>

모든 화소에 대해 거리를 구하기 위해서 euclidian distance를 구하였다. 해당 코드는 다음과 같다. 먼저 한 화소에 대해 모든 클러스터의 중심과의 거리를 구한다. k개의 클러스터 중심과의 거리가 가장 짧은 클러스터로 해당 화소를 배정한다. 이를 모든 화소에 대해 반복한다.

~~~c++

for (int i = 0; i < n; i++) {		
    int min_dist = 1e5;
	int min_idx = -1;

	for (int j = 0; j < k; j++) {
		float dist = get_distance_3b(v[i],cluster_center[j]);
		if (dist < min_dist) {
			min_idx = j;
			min_dist = dist;				
            }
		}
	cluster[min_idx].push_back(v[i]);
	}
~~~
<br/>
<br/>

모든 화소에 대해 클러스터 배정이 완료되었으면 클러스터 중심점을 업데이트한다. 이 때, 이전 클러스터 중심점과 바뀐 클러스터 중심점의 차이가 거의 없다면(여기에서는 RGB 합해서 6개 이하) 군집화가 수렴되었다고 판단하고 군집화를 종료한다. 코드에서는 바뀐 클러스터 중심점을 Vec3i로 선언하였다. 그 이유는 uchar overflow 때문이다. 특정 클러스터의 모든 화소값을 더하고 마지막에 클러스터의 개수로 나누기 때문에 Vec3b로 선언할 경우 오버플로우가 날 수 있다.

~~~c++
for (int i = 0; i < k; i++) {
	int size_ = cluster[i].size();
	Vec3i after_center = { 0,0,0 }; //오버플로우 방지
	Vec3b before_center;
	for (int j = 0; j < size_; j++) {
		after_center[0] += cluster[i][j][0];
		after_center[1] += cluster[i][j][1];
		after_center[2] += cluster[i][j][2];
	}
	before_center = cluster_center[i];

	after_center /= size_;

	//기존 군집 내 샘플 평균과 비교하기
	int cnt = 0;
	for (int j = 0; j < 3; j++) {
		if (abs(before_center[j] - after_center[j]) <= 2) cnt++;
	}
	cluster_center[i] = after_center; //샘플 평균 업데이트
}

//모든 군집의 갯수의 변화가 없으면 군집화 종료
//~~~~~//
~~~


군집화가 종료되었으면 군집 내의 모든 화소를 같은 색으로 지정해야한다. 이를 위해 전체 화소 3차원 배열을 선언하여, 클러스터 내 모든 화소에 같은 번호를 매겼다.

~~~c++
//모든 화소를 각 군집 번호로 바꾸기
int pixel[256][256][256] = {};

for (int i = 0; i < k; i++) {
	int size_ = cluster[i].size();
	for (int j = 0; j < size_; j++) {
		uchar b = cluster[i][j][0];
		uchar g = cluster[i][j][1];
		uchar r = cluster[i][j][2];
		pixel[b][g][r] = i + 1;
	}
}
~~~
<br/>
<br/>
이후 같은 번호이면 같은 색을 부여했다. 여기에서는  Red,Green,Blue,Orange 최대 4가지 색으로 하였다. 이 방법 말고, 클러스터 내의 색들을 유사성을 가지고 있으므로 R,G,B,O가 아닌 클러스터의 중심점의 색으로 하면 군집 번호 매기는 과정이 생략될 것이다.

~~~c++
//색 매기기
	for (int j = 0; j < img.cols-1; j++) {
		for (int i = 0; i < img.rows-1; i++) {
			uchar img_b = img.at<Vec3b>(i, j)[0];
			uchar img_g = img.at<Vec3b>(i, j)[1];
			uchar img_r = img.at<Vec3b>(i, j)[2];
			switch (pixel[img_b][img_g][img_r]) {
			case 1: out.at<Vec3b>(i, j) = { 0,0,255 }; break;
			case 2: out.at<Vec3b>(i, j) = { 255,0,0 }; break;
			case 3: out.at<Vec3b>(i, j) = { 0,255,0 }; break;
			case 4: out.at<Vec3b>(i, j) = { 0,123,255 }; break;
			default: break;
			}
		}
	}
~~~ 

---

한계점
---
K-means는 사용자가 직접 영상에서 k개의 군집을 정해줘야하는 단점이 있다.
또한 군집화 과정이 화소 사이의 지리적 근접성을 무시하는 결과를 가져온다. 화소의 컬러값의 유사성만 고려할 뿐 두 화소의 거리는 전혀 고려하지 않기 때문에 하나의 물체가 다양한 색을 가지고 있을 경우 전체적으로 하나의 물체라고 군집화하는 것이 아니라, 각각 다른 물체가 된다. 예를 들어 화려한 꽃의 경우 꽃잎과 줄기가 모두 다른 색으로 군집을 형성한다.


문의는 wpql2057@naver.com