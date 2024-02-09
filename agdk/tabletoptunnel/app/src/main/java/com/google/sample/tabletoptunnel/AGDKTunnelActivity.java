/*
 * Copyright 2021 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.google.sample.tabletoptunnel;

import static android.view.inputmethod.EditorInfo.IME_ACTION_NONE;
import static android.view.inputmethod.EditorInfo.IME_FLAG_NO_FULLSCREEN;

import android.app.Activity;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.os.Build.VERSION;
import android.os.Build.VERSION_CODES;
import android.os.Bundle;

import android.os.Handler;
import android.os.Looper;
import android.text.InputType;
import android.util.Log;
import android.view.View;
import android.view.WindowManager.LayoutParams;

import androidx.annotation.Keep;
import androidx.core.util.Consumer;
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.core.view.WindowInsetsControllerCompat;

import androidx.appcompat.app.AppCompatActivity;
import androidx.constraintlayout.widget.ConstraintSet;
import androidx.lifecycle.Lifecycle;
import androidx.window.java.layout.WindowInfoTrackerCallbackAdapter;
import androidx.window.layout.DisplayFeature;
import androidx.window.layout.FoldingFeature;
import androidx.window.layout.WindowInfoTracker;
import androidx.window.layout.WindowLayoutInfo;
import androidx.window.layout.WindowMetrics;
import androidx.window.layout.WindowMetricsCalculator;

import com.google.android.games.basegameframework.BaseGameFrameworkUtils;
import com.google.androidgamesdk.GameActivity;
import com.google.android.libraries.play.games.inputmapping.InputMappingClient;
import com.google.android.libraries.play.games.inputmapping.InputMappingProvider;
import com.google.android.libraries.play.games.inputmapping.Input;

import java.util.List;
import java.util.concurrent.Executor;

@Keep
public class AGDKTunnelActivity extends GameActivity {

    private PGSManager mPGSManager;
    private final String mPlayGamesPCSystemFeature =
            "com.google.android.play.feature.HPE_EXPERIENCE";
    private static final String TAG = "AGDKTunnelActivity";

    private WindowInfoTrackerCallbackAdapter windowInfoTrackerCallbackAdapter;

    private WindowMetricsCalculator windowMetricsCalculator;
    private LayoutStateChangeCallback layoutStateChangeCallback;

    private static MyExecutor executor;

    // Some code to load our native library:
    static {
        // Load the STL first to workaround issues on old Android versions:
        // "if your app targets a version of Android earlier than Android 4.3
        // (Android API level 18),
        // and you use libc++_shared.so, you must load the shared library before any other
        // library that depends on it."
        // See https://developer.android.com/ndk/guides/cpp-support#shared_runtimes
        System.loadLibrary("c++_shared");

        // Optional: reload the native library.
        // However this is necessary when any of the following happens:
        //     - tabletoptunnel library is not configured to the following line in the manifest:
        //        <meta-data android:name="android.app.lib_name" android:value="tabletoptunnel" />
        //     - GameActivity derived class calls to the native code before calling
        //       the super.onCreate() function.
        System.loadLibrary("tabletoptunnel");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d(TAG, TAG + ".onCreate");
        baseGameFrameworkUtils = new BaseGameFrameworkUtils(this);
        baseGameFrameworkUtils.Initialize();

        // When true, the app will fit inside any system UI windows.
        // When false, we render behind any system UI windows.
        WindowCompat.setDecorFitsSystemWindows(getWindow(), false);
        hideSystemUI();
        // You can set IME fields here or in native code using GameActivity_setImeEditorInfoFields.
        // We set the fields in native_engine.cpp.
        // super.setImeEditorInfoFields(InputType.TYPE_CLASS_TEXT,
        //     IME_ACTION_NONE, IME_FLAG_NO_FULLSCREEN );
        super.onCreate(savedInstanceState);

        if (isPlayGamesServicesLinked()) {
            // Initialize Play Games Services
            mPGSManager = new PGSManager(this);
        }

        if (isGooglePlayGames()) {
            InputMappingProvider inputMappingProvider = new InputSDKProvider();
            InputMappingClient inputMappingClient = Input.getInputMappingClient(this);
            inputMappingClient.registerRemappingListener(new InputSDKRemappingListener());
            inputMappingClient.setInputMappingProvider(inputMappingProvider);
        }

        windowInfoTrackerCallbackAdapter = new WindowInfoTrackerCallbackAdapter(WindowInfoTracker.Companion.getOrCreate(this));
        windowMetricsCalculator = WindowMetricsCalculator.getOrCreate();
        layoutStateChangeCallback = new LayoutStateChangeCallback(this);

        executor = new MyExecutor();

        obtainWindowMetrics();
        onWindowLayoutInfoChange();
    }

    private void obtainWindowMetrics() {
        WindowMetrics currentMetrics = windowMetricsCalculator.computeCurrentWindowMetrics(this);
        WindowMetrics maxMetrics = windowMetricsCalculator.computeMaximumWindowMetrics(this);
        Log.d(TAG, "WindowMetrics current: " + currentMetrics.toString());
        Log.d(TAG, "WindowMetrics max: " + maxMetrics.toString());
        WindowInfoTracker windowInfoTracker = WindowInfoTracker.Companion.getOrCreate(this);
        // Flow<WindowLayoutInfo> windowLayoutInfos = windowInfoTracker.windowLayoutInfo(this);
        
        // List<DisplayFeature> displayFeatures = windowLayoutInfo.getDisplayFeatures();
        // if ( !displayFeatures.isEmpty() ) {
        //     for (DisplayFeature displayFeature : displayFeatures ) {
        //         FoldingFeature foldingFeature = (FoldingFeature) displayFeature;
        //         if ( foldingFeature != null ) {
        //             Log.d(TAG, foldingFeature.toString());
        //             Log.d(TAG, "Folding Feature orientation: " + foldingFeature.getOrientation());
        //             Log.d(TAG, "Folding Feature state: " + foldingFeature.getState());
        //             Log.d(TAG, "Folding Feature occlusionType: " + foldingFeature.getOcclusionType());
        //             Log.d(TAG, "Folding Feature isSeperating: " + foldingFeature.isSeparating());
        //             Log.d(TAG, "Folding Feature bounds: " + foldingFeature.getBounds());
        //         }
        //     }
        // }
    }

    private void onWindowLayoutInfoChange() {

    }

    @Override
    protected void onDestroy() {
        Log.d(TAG, TAG + ".onDestroy");
        if (isGooglePlayGames()) {
            InputMappingClient inputMappingClient = Input.getInputMappingClient(this);
            inputMappingClient.clearInputMappingProvider();
            inputMappingClient.clearRemappingListener();
        }

        super.onDestroy();
    }

    @Override
    protected void onStart() {
        Log.d(TAG, TAG + ".onStart");

        super.onStart();

        // FOLDABLES
        windowInfoTrackerCallbackAdapter.addWindowLayoutInfoListener(this, runOnUiThreadExecutor(), layoutStateChangeCallback);
    }

    @Override
    protected void onStop() {
        Log.d(TAG, TAG + ".onStop");

        super.onStop();

        // FOLDABLES
        windowInfoTrackerCallbackAdapter.removeWindowLayoutInfoListener(layoutStateChangeCallback);
    }

    @Override
    protected void onPause() {
        Log.d(TAG, TAG + ".onPause");

        super.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();

        Log.d(TAG, TAG + ".onResume");

        // To learn best practices to handle lifecycle events visit
        // https://developer.android.com/topic/libraries/architecture/lifecycle
        if (isPlayGamesServicesLinked()) {
            mPGSManager.onResume();
        }
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        Log.d(TAG, TAG + ".onWindowFocusChanged " + hasFocus);
        super.onWindowFocusChanged(hasFocus);
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        Log.d(TAG, TAG + ".onConfigurationChanged " + newConfig.toString());
    }

    private void hideSystemUI() {
        // This will put the game behind any cutouts and waterfalls on devices which have
        // them, so the corresponding insets will be non-zero.
        if (VERSION.SDK_INT >= VERSION_CODES.P) {
            getWindow().getAttributes().layoutInDisplayCutoutMode
                = LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_ALWAYS;
        }
        // From API 30 onwards, this is the recommended way to hide the system UI, rather than
        // using View.setSystemUiVisibility.
        View decorView = getWindow().getDecorView();
        WindowInsetsControllerCompat controller = new WindowInsetsControllerCompat(getWindow(),
            decorView);
        controller.hide(WindowInsetsCompat.Type.systemBars());
        controller.hide(WindowInsetsCompat.Type.displayCutout());
        controller.setSystemBarsBehavior(
            WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
    }

    private boolean isPlayGamesServicesLinked() {
        String playGamesServicesPlaceholder = "0000000000";
        return !getString(R.string.game_services_project_id).equals(playGamesServicesPlaceholder);
    }

    private void loadCloudCheckpoint() {
        if (isPlayGamesServicesLinked()) {
            mPGSManager.loadCheckpoint();
        }
    }

    private void saveCloudCheckpoint(int level) {
        if (isPlayGamesServicesLinked()) {
            mPGSManager.saveCheckpoint(level);
        }
    }

    private String getInternalStoragePath() {
        return getFilesDir().getAbsolutePath();
    }

    private void setInputContext(int contextIndex) {
        for(InputSDKProvider.InputContextIds context : InputSDKProvider.InputContextIds.values()) {
            if (context.value() == contextIndex) {
                setInputContext(context);
                return;
            }
        }
        Log.e(TAG, String.format(
                "can't find InputContext with id %d on attempt to change of context",
                contextIndex));
    }

    private void setInputContext(InputSDKProvider.InputContextIds context) {
        InputMappingClient inputMappingClient = Input.getInputMappingClient(this);
        switch(context) {
            case INPUT_CONTEXT_PLAY_SCENE:
                inputMappingClient.setInputContext(InputSDKProvider.sPlaySceneInputContext);
                break;
            case INPUT_CONTEXT_UI_SCENE:
                inputMappingClient.setInputContext(InputSDKProvider.sUiSceneInputContext);
                break;
            case INPUT_CONTEXT_PAUSE_MENU:
                inputMappingClient.setInputContext(InputSDKProvider.sPauseMenuInputContext);
                break;
            default:
                Log.e(TAG,
                        "can't match the requested InputContext on attempt to change of context");
        }
    }

    private boolean isGooglePlayGames() {
        PackageManager pm = getPackageManager();
        return pm.hasSystemFeature(mPlayGamesPCSystemFeature);
    }

    private BaseGameFrameworkUtils baseGameFrameworkUtils;

    // FOLDABLES
    static Executor runOnUiThreadExecutor() {
        return executor;
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

        private final WindowMetricsCalculator windowMetricsCalculator;

        public LayoutStateChangeCallback(Activity activity) {
            this.activity = activity;
            this.windowMetricsCalculator = WindowMetricsCalculator.getOrCreate();
        }

        @Override
        public void accept(WindowLayoutInfo windowLayoutInfo) {
            // updateLayout
            Log.d(TAG, "LayoutStateChangeCallback accept");
            WindowMetrics currentMetrics = windowMetricsCalculator.computeCurrentWindowMetrics(activity);
            WindowMetrics maxMetrics = windowMetricsCalculator.computeMaximumWindowMetrics(activity);

            Log.d(TAG, "FLOW WindowMetrics current: " + currentMetrics.getBounds().toString() + " windowInsets: " + currentMetrics.getWindowInsets().toWindowInsets().toString());
            Log.d(TAG, "FLOW WindowMetrics max: " + maxMetrics.getBounds().toString() + " windowInsets: " + maxMetrics.getWindowInsets().toWindowInsets().toString());

            /*
                2024-02-08 19:17:36.628 32006-32006 AGDKTunnelActivity      com.google.sample.tabletoptunnel     D  LayoutStateChangeCallback accept
                2024-02-08 19:17:36.634 32006-32006 AGDKTunnelActivity      com.google.sample.tabletoptunnel     D  HardwareFoldingFeature { Bounds { [1104,0,1104,1840] }, type=FOLD, state=HALF_OPENED }
                2024-02-08 19:17:36.635 32006-32006 AGDKTunnelActivity      com.google.sample.tabletoptunnel     D  Folding Feature orientation: VERTICAL
                2024-02-08 19:17:36.635 32006-32006 AGDKTunnelActivity      com.google.sample.tabletoptunnel     D  Folding Feature state: HALF_OPENED
                2024-02-08 19:17:36.635 32006-32006 AGDKTunnelActivity      com.google.sample.tabletoptunnel     D  Folding Feature occlusionType: NONE
                2024-02-08 19:17:36.635 32006-32006 AGDKTunnelActivity      com.google.sample.tabletoptunnel     D  Folding Feature isSeperating: true
                2024-02-08 19:17:36.635 32006-32006 AGDKTunnelActivity      com.google.sample.tabletoptunnel     D  Folding Feature bounds: Rect(1104, 0 - 1104, 1840)
             */

            /*
                2024-02-08 19:23:31.804   679-773   GameControllerThread    com.google.sample.tabletoptunnel     D  onInputDeviceChanged id: 5
                2024-02-08 19:23:31.807   679-679   AGDKTunnelActivity      com.google.sample.tabletoptunnel     D  HardwareFoldingFeature { Bounds { [0,797,1840,797] }, type=FOLD, state=HALF_OPENED }
                2024-02-08 19:23:31.807   679-679   AGDKTunnelActivity      com.google.sample.tabletoptunnel     D  Folding Feature orientation: HORIZONTAL
                2024-02-08 19:23:31.807   679-679   AGDKTunnelActivity      com.google.sample.tabletoptunnel     D  Folding Feature state: HALF_OPENED
                2024-02-08 19:23:31.807   679-679   AGDKTunnelActivity      com.google.sample.tabletoptunnel     D  Folding Feature occlusionType: NONE
                2024-02-08 19:23:31.807   679-679   AGDKTunnelActivity      com.google.sample.tabletoptunnel     D  Folding Feature isSeperating: true
                2024-02-08 19:23:31.807   679-679   AGDKTunnelActivity      com.google.sample.tabletoptunnel     D  Folding Feature bounds: Rect(0, 797 - 1840, 797)
                2024-02-08 19:23:31.932   679-679   AGDKTunnelActivity      com.google.sample.tabletoptunnel     D  LayoutStateChangeCallback accept
                2024-02-08 19:23:31.937   679-773   GameControllerThread    com.google.sample.tabletoptunnel     D  onInputDeviceChanged id: 5
                2024-02-08 19:23:31.938   679-679   AGDKTunnelActivity      com.google.sample.tabletoptunnel     D  HardwareFoldingFeature { Bounds { [0,797,1840,797] }, type=FOLD, state=FLAT }
                2024-02-08 19:23:31.938   679-679   AGDKTunnelActivity      com.google.sample.tabletoptunnel     D  Folding Feature orientation: HORIZONTAL
                2024-02-08 19:23:31.938   679-679   AGDKTunnelActivity      com.google.sample.tabletoptunnel     D  Folding Feature state: FLAT
                2024-02-08 19:23:31.938   679-679   AGDKTunnelActivity      com.google.sample.tabletoptunnel     D  Folding Feature occlusionType: NONE
                2024-02-08 19:23:31.938   679-679   AGDKTunnelActivity      com.google.sample.tabletoptunnel     D  Folding Feature isSeperating: false
                2024-02-08 19:23:31.938   679-679   AGDKTunnelActivity      com.google.sample.tabletoptunnel     D  Folding Feature bounds: Rect(0, 797 - 1840, 797)
             */
            List<DisplayFeature> displayFeatures = windowLayoutInfo.getDisplayFeatures();
            if ( !displayFeatures.isEmpty() ) {
                for (DisplayFeature displayFeature : displayFeatures ) {
                    FoldingFeature foldingFeature = (FoldingFeature) displayFeature;
                    if ( foldingFeature != null ) {
                        Log.d(TAG, foldingFeature.toString());
                        Log.d(TAG, "FLOW Folding Feature orientation: " + foldingFeature.getOrientation());
                        Log.d(TAG, "FLOW Folding Feature state: " + foldingFeature.getState());
                        Log.d(TAG, "FLOW Folding Feature occlusionType: " + foldingFeature.getOcclusionType());
                        Log.d(TAG, "FLOW Folding Feature isSeperating: " + foldingFeature.isSeparating());
                        Log.d(TAG, "FLOW Folding Feature bounds: " + foldingFeature.getBounds());
                    }
                }
            }
        }
    }
}
