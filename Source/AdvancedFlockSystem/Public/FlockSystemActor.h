// Copyright 2020-2021 Fly Dream Dev. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Core/Public/HAL/Runnable.h"
#include "Runtime/Core/Public/HAL/ThreadSafeBool.h"
#include "FlockSystemActor.generated.h"

USTRUCT(BlueprintType)
struct FlockMemberData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY()
    int instanceIndex = 0;

    UPROPERTY()
    FVector velocity = FVector::ZeroVector;

    UPROPERTY()
    FVector wanderPosition = FVector::ZeroVector;

    UPROPERTY()
    FTransform transform;

    UPROPERTY()
    float elapsedTimeSinceLastWander = 0.f;

    UPROPERTY()
    bool bIsFlockLeader = false;

    UPROPERTY()
    TArray<AActor*> attackedActors;

    FlockMemberData()
    {
        instanceIndex = 0;
        velocity = FVector::ZeroVector;
        elapsedTimeSinceLastWander = 0.0f;
        wanderPosition = FVector::ZeroVector;
        bIsFlockLeader = false;
    };
};

USTRUCT(BlueprintType)
struct FlockMembersArrays
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY()
    int instanceIndex = 0;

    UPROPERTY()
    TArray<FlockMemberData> flockMembersArr;

    FlockMembersArrays()
    {
        instanceIndex = 0;
        flockMembersArr.Empty();
    };

    void AddData(int index, TArray<FlockMemberData> newFlockData)
    {
        instanceIndex = index;
        flockMembersArr = newFlockData;
    };
};

UENUM(BlueprintType)
enum class EPriority: uint8
{
    Normal UMETA(DisplayName = "普通"),
    AboveNormal UMETA(DisplayName = "高于普通"),
    BelowNormal UMETA(DisplayName = "低于普通"),
    Highest UMETA(DisplayName = "最高"),
    Lowest UMETA(DisplayName = "最低"),
    SlightlyBelowNormal UMETA(DisplayName = "略低于普通"),
    TimeCritical UMETA(DisplayName = "时间关键")
};

USTRUCT(BlueprintType)
struct FlockMemberParameters
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|生成设置",
        meta=(DisplayName="线程优先级", ToolTip="设置群体计算线程的优先级"))
    EPriority threadPriority = EPriority::Normal;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|生成设置",
        meta=(DisplayName="使用单一领导者", ToolTip="如果启用，所有群体成员将跟随一个领导者"))
    bool bUseOneLeader = false;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|生成设置",
        meta=(DisplayName="跟随目标Actor", ToolTip="群体将跟随指定的Actor移动"))
    AActor* followActor = nullptr;
    
    // Use for only more flock mates!
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|生成设置",
        meta=(DisplayName="实验性优化", ToolTip="仅适用于大量群体成员，启用后可提升性能"))
    bool ExperimentalOptimization = false;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|生成设置",
        meta=(DisplayName="使用水族箱边界", ToolTip="如果启用，群体将在BoxComponent范围内活动"))
    bool bUseAquarium = true;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|生成设置",
        meta=(DisplayName="随机游荡半径", ToolTip="领导者随机移动的范围半径"))
    float flockWanderInRandomRadius = 3000.0f;
    // If you want add all not mobile actors for avoidance. 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "群体系统|行为参数",
        meta=(DisplayName="自动添加避障组件", ToolTip="自动将所有非移动Actor添加到避障数组中"))
    bool bAutoAddComponentsInArray = true;
    
    // If need react on pawn.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "群体系统|行为参数",
        meta=(DisplayName="响应Pawn", ToolTip="群体将对Pawn角色做出反应（逃离）"))
    bool bReactOnPawn = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "群体系统|行为参数",
        meta=(DisplayName="跟随Pawn", ToolTip="群体将跟随Pawn角色移动"))
    bool bFollowToPawn = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "群体系统|行为参数",
        meta=(DisplayName="可攻击Pawn", ToolTip="群体可以攻击Pawn角色"))
    bool bCanAttackPawn = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "群体系统|行为参数",
        meta=(DisplayName="攻击半径", ToolTip="群体攻击Pawn的有效范围"))
    float attackRadius = 100.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "群体系统|飞虫参数",
        meta=(DisplayName="伤害值", ToolTip="每次攻击造成的伤害量"))
    float damageValue = 0.001f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "群体系统|飞虫参数",
        meta=(DisplayName="伤害类型", ToolTip="攻击时应用的伤害类型类"))
    TSubclassOf<class UDamageType> damageType;
    
    UPROPERTY()
    float attackRadiusSquared = 0.f;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|生成设置",
        meta=(DisplayName="使用最大高度", ToolTip="限制群体移动的最大高度"))
    bool bUseMaxHeight = false;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|生成设置",
        meta=(DisplayName="最大高度", ToolTip="群体可移动的最高Z轴位置"))
    float maxHeight = 0.f;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|生成设置",
        meta=(DisplayName="线程内移动插值速度", ToolTip="线程中位置插值的速度系数"))
    float moveSpeedInterpInThread = 5.f;
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|行为参数",
        meta=(DisplayName="群体最大速度", ToolTip="群体成员移动的最大速度"))
    float flockMaxSpeed = 40.0f;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|行为参数",
        meta=(DisplayName="速度偏移量", ToolTip="随机速度变化范围"))
    float flockOffsetSpeed = 5.0f;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|行为参数",
        meta=(DisplayName="逃逸速度倍数", ToolTip="逃离危险时速度的乘数"))
    float escapeMaxSpeedMultiply = 2.0f;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|行为参数",
        meta=(DisplayName="跟随强度", ToolTip="跟随领导者或目标的力量系数"))
    float followScale = 1.0f;
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|行为参数",
        meta=(DisplayName="群体感知半径", ToolTip="群体成员感知其他成员的范围"))
    float flockMateAwarenessRadius = 400.0f;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|行为参数",
        meta=(DisplayName="对齐强度", ToolTip="群体成员对齐方向的强度"))
    float alignScale = 0.4f;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|行为参数",
        meta=(DisplayName="凝聚强度", ToolTip="群体成员聚集在一起的强度"))
    float cohesionScale = 0.6f;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|行为参数",
        meta=(DisplayName="分离强度", ToolTip="群体成员避免相互碰撞的强度"))
    float separationScale = 5.0f;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|行为参数",
        meta=(DisplayName="逃离强度", ToolTip="群体逃离危险的强度"))
    float fleeScale = 10.0f;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|行为参数",
        meta=(DisplayName="避障逃离强度", ToolTip="避开障碍物时的逃离强度"))
    float fleeScaleAvoidance = 10.0f;

    float avoidancePrimitiveDistance = 50.f;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|行为参数",
        meta=(DisplayName="分离半径", ToolTip="群体成员之间保持的最小距离"))
    float separationRadius = 6.0f;
    
    // Delay before leader find new wander location.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|行为参数",
        meta=(DisplayName="游荡更新频率", ToolTip="领导者寻找新游荡位置的时间间隔（秒）"))
    float flockWanderUpdateRate = 2.0f;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|行为参数",
        meta=(DisplayName="最小游荡距离", ToolTip="领导者游荡的最小距离"))
    float flockMinWanderDistance = 50.0f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|行为参数",
        meta=(DisplayName="群体旋转速度", ToolTip="群体成员正常移动时的旋转速度"))
    float flockMateRotationRate = 0.6f;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|行为参数",
        meta=(DisplayName="逃逸旋转速度", ToolTip="群体逃离危险时的旋转速度"))
    float escapeMateRotationRate = 10.f;

    float flockMaxSteeringForce = 100.0f;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|行为参数",
        meta=(DisplayName="敌人感知半径", ToolTip="群体感知敌人的范围"))
    float flockEnemyAwarenessRadius = 200.0f;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|行为参数",
        meta=(DisplayName="跟随Pawn感知半径", ToolTip="感知Pawn并跟随的范围"))
    float followPawnAwarenessRadius = 500.0f;

    float fleeScaleAquarium = 10.0f;
    float strengthAquariumOffsetValue = 100.f;


    FlockMemberParameters()
    {
        attackRadiusSquared = FMath::Square(attackRadius);
    };
};

UCLASS()
class ADVANCEDFLOCKSYSTEM_API AFlockSystemActor : public AActor
{
    GENERATED_BODY()

public:


    // constructor
    AFlockSystemActor();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    //************************************************************************
    // Component                                                                  
    //************************************************************************

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "群体系统|组件",
        meta=(DisplayName="球形碰撞体", ToolTip="用于定位的球形碰撞组件"))
    class USphereComponent* SphereComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "群体系统|组件",
        meta=(DisplayName="盒形碰撞体", ToolTip="定义群体活动范围的盒形碰撞组件"))
    class UBoxComponent* BoxComponent;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "群体系统|组件",
        meta=(DisplayName="实例化静态网格组件", ToolTip="用于渲染群体成员的实例化网格组件"))
    class UInstancedStaticMeshComponent* StaticMeshInstanceComponent;

    //************************************************************************

    void GenerateFlockThread();

    void AddAvoidanceComponentsTimer();

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|生成设置",
        meta=(DisplayName="静态网格", ToolTip="群体成员使用的网格模型"))
    UStaticMesh* StaticMesh;
    
    // Recommended - 1 Thread = (2000 - 2500) mates. 
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|生成设置",
        meta=(ClampMin="1", ClampMax="32", DisplayName="最大使用线程数", ToolTip="建议：1个线程处理2000-2500个群体成员"))
    int maxUseThreads = 1;
    
    // Recommended - 1 Thread = (2000 - 2500) mates.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|生成设置",
        meta=(DisplayName="群体成员数量", ToolTip="要生成的群体成员总数"))
    int32 flockMateInstances = 1000;
    
    // Random mesh scale.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|生成设置",
        meta=(DisplayName="最小网格缩放", ToolTip="随机网格缩放的最小值"))
    float minMeshScale = 1.f;
    
    // Random mesh scale.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|生成设置",
        meta=(DisplayName="最大网格缩放", ToolTip="随机网格缩放的最大值"))
    float maxMeshScale = 2.f;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|动画参数",
        meta=(DisplayName="移动插值速率", ToolTip="群体成员位置插值的动画速度"))
    float interpMoveAnimRate = 200.f;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统|动画参数",
        meta=(DisplayName="旋转插值速率", ToolTip="群体成员旋转插值的动画速度"))
    float interpRotateAnimRate = 25.f;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "群体系统",
        meta=(DisplayName="群体成员参数", ToolTip="群体行为的详细参数配置"))
    FlockMemberParameters flockParameters;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "群体系统|避障",
        meta=(DisplayName="避障Actor数组", ToolTip="手动指定需要避开的Actor根组件"))
    TArray<AActor*> avoidanceActorRootArr;
    
    UPROPERTY(BlueprintReadOnly, Category = "群体系统|运行时数据",
        meta=(DisplayName="群体成员数据", ToolTip="运行时群体成员的数据数组"))
    TArray<FlockMemberData> mFlockMemberData;
    // Add an instance to this component. Transform is given in world space. 
    void AddFlockMemberWorldSpace(const FTransform& WorldTransform);

    int32 numFlock;

    UPROPERTY()
    TArray<UPrimitiveComponent*> allOverlappingComponentsArr;

    TArray<FlockMembersArrays> allFlockMembersArrays;

    void DivideFlockArrayForThreads();

    UPROPERTY()
    TArray<AActor*> dangerActors;

protected:

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual void BeginDestroy() override;

    FTimerHandle addAvoidanceActor_Timer;

private:

    TArray<class FlockThread*> FlockActorPoolThreadArr;

};

// Thread
class FlockThread : public FRunnable
{
public:

    //================================= THREAD =====================================
    
    //Constructor
    FlockThread(AActor *newActor);
    //Destructor
    virtual ~FlockThread() override;

    //Use this method to kill the thread!!
    void EnsureCompletion();
    //Pause the thread 
    void PauseThread();
    //Continue/UnPause the thread
    void ContinueThread();
    //FRunnable interface.
    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;
    bool IsThreadPaused() const;

    //================================= FLOCK =====================================
    class UBoxComponent* BoxComponentRef;

    TArray<FlockMemberData> GetFlockMembersData();

    void InitFlockParameters(TArray<FlockMemberData> setFlockMembersArr, FlockMemberParameters newParameters, UBoxComponent* SetBoxComponent);

    void SetOverlappingComponents(TArray<UPrimitiveComponent*> overlappingComponentsArr, TArray<AActor*> dangerActors);
    void SetAvoidanceActor(TArray<AActor*> avoidanceActorRootArr);

    FVector SteeringAquarium(const FlockMemberData& flockMember) const;
    FVector SteeringAvoidanceComponent(const FlockMemberData& flockMember) const;
    FVector SteeringWander(FlockMemberData& flockMember) const;
    FVector GetRandomWanderLocation() const;
    FVector SteeringFollow(FlockMemberData& flockMember, int32 flockLeader);
    TArray<int32> GetNearbyFlockMates(int32 flockMember);
    FVector SteeringAlign(FlockMemberData& flockMember, TArray<int32>& flockMates);
    FVector SteeringSeparate(const FlockMemberData& flockMember, TArray<int32>& flockMates);
    FVector SteeringCohesion(const FlockMemberData& flockMember, TArray<int32>& flockMates);
    FVector SteeringFlee(const FlockMemberData& flockMember);
    FVector SteeringAvoidance(const FlockMemberData& flockMember) const;
    FVector SteeringMaxHeight(const FlockMemberData& flockMember) const;
    FVector SteeringFollowPawn(FlockMemberData& flockMember);

    void InitFlockLeader();

    TArray<FlockMemberData> flockThreadMembersArr;
    FlockMemberParameters flockParametersTHR;
    TArray<UPrimitiveComponent*> allOverlappingComponentsArrTHR;
    TArray<AActor*> dangerActorsTHR;
    TArray<AActor*> avoidanceActorRootArrTHR;

    float threadDeltaTime = 0.f;
    float lastFrameTime = 0.f;
    //================================= FLOCK =====================================

    void SetPoolThread(TArray<class FlockThread*> setPoolThreadArr);

    AActor *ownerActor;
    

private:

    //Thread to run the worker FRunnable on
    FRunnableThread* Thread;

    FCriticalSection m_mutex;
    FEvent* m_semaphore;

    int m_chunkCount;
    int m_amount;
    int m_MinInt;
    int m_MaxInt;

    float threadSleepTime = 0.01f;

    //As the name states those members are Thread safe
    FThreadSafeBool m_Kill;
    FThreadSafeBool m_Pause;

    TArray<class FlockThread*> PoolThreadArr;
};
