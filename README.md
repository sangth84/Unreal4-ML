# Unreal4-ML

![image](https://github.com/sangth84/Unreal4-ML/assets/40972832/d43abb7e-1821-4565-9b3f-3d3271eb2632)

# 샘플 동작 방법
1.	Plugins\EgLabRL\Server\ml-remote-server 폴더에 있는 StartupServerByPython37.bat을 실행 시킵니다.
2.	월드에 배치된 액터를 선택하고 설정 값을 변경 후 학습을 시작 합니다.
( 학습 시작 전 이전에 생성된 Plugins\EgLabRL\Server\ml-remote-server\scripts\ Saved 폴더를 지워 줍니다. )

![image](https://github.com/sangth84/Unreal4-ML/assets/40972832/632cdc06-e67e-403f-9f6a-b80cbc74eedd)

3.	게임을 실행하면 학습이 시작됩니다.
4.	Plugins\EgLabRL\Server\ml-remote-server\scripts\dopamine\agents 하위에 있는 Config 폴더에 들어가 gin 파일을 열어서 하이퍼 파라미터를 변경 가능합니다.

# 타 프로젝트에 적용 방법
1.	EglabMLMgr_Blueprint를 레벨에 배치합니다.
2.	IRLControllerInterface 컨트롤러를 상속받아서 가상함수들을 구현해 줍니다.
3.	Observation Box들은 오버랩처리로 구현되어 있기 때문에 게임에서 오버랩이 되지 않도록 처리가 필요합니다.
샘플 프로젝트에 PlayerProjectile BP의 이벤트 그래프를 참고하세요.

![image](https://github.com/sangth84/Unreal4-ML/assets/40972832/f173f6ad-6285-4a5b-b242-5860592bde4c)

4.	게임이 리셋 되었을 때, EglabManager에 EndGameNotification을 호출해 주세요.
샘플 프로젝트에 GeoGameState의 이벤트 그래프를 참고해주세요.

![image](https://github.com/sangth84/Unreal4-ML/assets/40972832/a55ea4ca-6206-49bc-aaa9-b9e1cab0a1af)

5.	보상 설정을 위해서 EglabManager에 GiveReward 설정을 해주세요.
샘플 프로젝트에 BP_MasterEnemy의 이벤트 그래프를 참고해주세요.

![image](https://github.com/sangth84/Unreal4-ML/assets/40972832/f06f95e7-a230-4b06-82e6-88281a10a021)

6.	Enemy와 Wall Acter에 Tag를 추가해주시면, 모든 설정이 완료됩니다.
7.	치트키( RL.DebugDrawObservationBox 1 )를 이용해 정상적으로 동작하고 있는지 확인이 가능합니다.
