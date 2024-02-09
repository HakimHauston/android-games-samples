package com.google.sample.tabletoptunnel.foldable;

//Android Imports
import android.app.Activity;
import android.graphics.Rect;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;

//Android Jetpack WindowManager Imports
import androidx.annotation.NonNull;
import androidx.core.util.Consumer;
import androidx.window.java.layout.WindowInfoTrackerCallbackAdapter;
import androidx.window.layout.DisplayFeature;
import androidx.window.layout.FoldingFeature;
import androidx.window.layout.WindowInfoTracker;
import androidx.window.layout.WindowLayoutInfo;
import androidx.window.layout.WindowMetrics;
import androidx.window.layout.WindowMetricsCalculator; 

//Java Imports
import java.util.List;
import java.util.concurrent.Executor;

public class FoldableHelper {

    private static LayoutStateChangeCallback layoutStateChangeCallback;
    private static WindowInfoTrackerCallbackAdapter wit;
    private static WindowMetricsCalculator wmc;

    public static native void OnLayoutChanged(FoldableLayoutInfo resultInfo);

    public static void init(Activity activity) {
        //Create Window Info Tracker
        wit = new WindowInfoTrackerCallbackAdapter(WindowInfoTracker.Companion.getOrCreate(activity));
        //Create Window Metrics Calculator
        wmc = WindowMetricsCalculator.Companion.getOrCreate();
        //Create Callback Object
        layoutStateChangeCallback = new LayoutStateChangeCallback(activity);
    }

    public static void start(Activity activity) {
        wit.addWindowLayoutInfoListener(activity, runOnUiThreadExecutor(), layoutStateChangeCallback);
    }

    public static void stop() {
        wit.removeWindowLayoutInfoListener(layoutStateChangeCallback);
    }

    static Executor runOnUiThreadExecutor() {
        return new MyExecutor();
     }
     
     static class MyExecutor implements Executor {
         Handler handler = new Handler(Looper.getMainLooper());
     
         @Override
         public void execute(Runnable command) {
             handler.post(command);
         }
     }

     static class LayoutStateChangeCallback implements Consumer<WindowLayoutInfo> {
        private final Activity activity;
    
        public LayoutStateChangeCallback(Activity activity) {
            this.activity = activity;
        }

        @Override
        public void accept(WindowLayoutInfo windowLayoutInfo) {
                
            FoldableLayoutInfo resultInfo = updateLayout(windowLayoutInfo, activity);

            OnLayoutChanged(resultInfo);
        }

        private static FoldableLayoutInfo updateLayout(WindowLayoutInfo windowLayoutInfo, Activity activity) {
            FoldableLayoutInfo retLayoutInfo = new FoldableLayoutInfo();

            WindowMetrics wm = wmc.computeCurrentWindowMetrics(activity);
            retLayoutInfo.currentMetrics = wm.getBounds();

            wm = wmc.computeMaximumWindowMetrics(activity);
            retLayoutInfo.maxMetrics = wm.getBounds();

            List<DisplayFeature> displayFeatures = windowLayoutInfo.getDisplayFeatures();
            if (!displayFeatures.isEmpty())
            {
                for (DisplayFeature displayFeature : displayFeatures)
                {
                    FoldingFeature foldingFeature = (FoldingFeature) displayFeature;
                    if (foldingFeature != null)
                    {  
                        if (foldingFeature.getOrientation() == FoldingFeature.Orientation.HORIZONTAL)
                        {
                        retLayoutInfo.hingeOrientation = FoldableLayoutInfo.HINGE_ORIENTATION_HORIZONTAL;
                        }
                        else
                        {
                        retLayoutInfo.hingeOrientation = FoldableLayoutInfo.HINGE_ORIENTATION_VERTICAL;
                        }

                        if (foldingFeature.getState() == FoldingFeature.State.FLAT)
                        {
                        retLayoutInfo.state = FoldableLayoutInfo.STATE_FLAT;
                        }
                        else
                        {
                        retLayoutInfo.state = FoldableLayoutInfo.STATE_HALF_OPENED;
                        }

                        if (foldingFeature.getOcclusionType() == FoldingFeature.OcclusionType.NONE)
                        {
                        retLayoutInfo.occlusionType = FoldableLayoutInfo.OCCLUSION_TYPE_NONE;
                        }
                        else
                        {
                        retLayoutInfo.occlusionType = FoldableLayoutInfo.OCCLUSION_TYPE_FULL;
                        }

                        retLayoutInfo.isSeparating = foldingFeature.isSeparating();

                        retLayoutInfo.bounds = foldingFeature.getBounds();

                        return retLayoutInfo;
                    }
                }
            }
        
            return retLayoutInfo;
        }

    }
    
}
