; ModuleID = 'test2.c'
source_filename = "test2.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @test() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 -5, i32* %1, align 4
  store i32 4, i32* %2, align 4
  store i32 5, i32* %3, align 4
  store i32 4, i32* %4, align 4
  br label %5

5:                                                ; preds = %24, %0
  %6 = load i32, i32* %1, align 4
  %7 = icmp slt i32 %6, 3
  br i1 %7, label %8, label %27

8:                                                ; preds = %5
  %9 = load i32, i32* %3, align 4
  %10 = sub nsw i32 %9, 1
  store i32 %10, i32* %3, align 4
  %11 = load i32, i32* %1, align 4
  %12 = add nsw i32 %11, 2
  store i32 %12, i32* %1, align 4
  %13 = load i32, i32* %2, align 4
  %14 = load i32, i32* %4, align 4
  %15 = icmp sgt i32 %13, %14
  br i1 %15, label %16, label %20

16:                                               ; preds = %8
  %17 = load i32, i32* %2, align 4
  %18 = load i32, i32* %4, align 4
  %19 = sub nsw i32 %17, %18
  store i32 %19, i32* %2, align 4
  br label %24

20:                                               ; preds = %8
  %21 = load i32, i32* %2, align 4
  %22 = load i32, i32* %4, align 4
  %23 = add nsw i32 %21, %22
  store i32 %23, i32* %2, align 4
  br label %24

24:                                               ; preds = %20, %16
  %25 = load i32, i32* %2, align 4
  %26 = sdiv i32 %25, 2
  store i32 %26, i32* %2, align 4
  br label %5, !llvm.loop !6

27:                                               ; preds = %5
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 14.0.0-1ubuntu1.1"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
