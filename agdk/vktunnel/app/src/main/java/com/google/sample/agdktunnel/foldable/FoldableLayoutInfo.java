package com.google.sample.tabletoptunnel.foldable;

import android.graphics.Rect;

public class FoldableLayoutInfo {
    public static int UNDEFINED = -1;

    // Hinge Orientation
    public static int HINGE_ORIENTATION_HORIZONTAL = 0;
    public static int HINGE_ORIENTATION_VERTICAL = 1;

    // State
    public static int STATE_FLAT = 0;
    public static int STATE_HALF_OPENED = 1;

    // Occlusion Type
    public static int OCCLUSION_TYPE_NONE = 0;
    public static int OCCLUSION_TYPE_FULL = 1;

    Rect currentMetrics = new Rect();
    Rect maxMetrics = new Rect();

    int hingeOrientation = UNDEFINED;
    int state = UNDEFINED;
    int occlusionType = UNDEFINED;
    boolean isSeparating = false;
    Rect bounds = new Rect();
}