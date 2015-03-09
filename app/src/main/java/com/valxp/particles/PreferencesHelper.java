package com.valxp.particles;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;

public class PreferencesHelper {
    public static final int settingsVersion = 1;

    private static Editor getEditor(Context ctx) {
        SharedPreferences prefs = getPrefs(ctx);
        Editor editor = prefs.edit();
        if (prefs.getInt(ctx.getString(R.string.settings_version), 0) < settingsVersion) {
            editor.clear();
            editor.putInt(ctx.getString(R.string.settings_version), settingsVersion);
            editor.commit();
            editor = null;
            editor = prefs.edit();
        }
        return editor;
    }

    private static SharedPreferences getPrefs(Context ctx) {
        SharedPreferences prefs = ctx.getSharedPreferences(ctx.getString(R.string.settings_name), 0);
        if (prefs.getInt(ctx.getString(R.string.settings_version), 0) < settingsVersion) {
            Editor editor = prefs.edit();
            editor.clear();
            editor.putInt(ctx.getString(R.string.settings_version), settingsVersion);
            editor.commit();
            editor = null;
        }
        return prefs;
    }

    public static void setParticleNumber(Context ctx, Long pnumber) {
        Editor edit = getEditor(ctx);
        edit.putLong(ctx.getString(R.string.settings_particle_number), pnumber);
        edit.commit();
    }

    public static void setParticleSize(Context ctx, float size) {
        Editor edit = getEditor(ctx);
        edit.putFloat(ctx.getString(R.string.settings_particle_size), size);
        edit.commit();
    }

    public static void setMotionBlur(Context ctx, boolean isEnabled) {
        Editor edit = getEditor(ctx);
        edit.putBoolean(ctx.getString(R.string.settings_particle_blur), isEnabled);
        edit.commit();
    }

    public static void setShowFps(Context ctx, boolean isEnabled) {
        Editor edit = getEditor(ctx);
        edit.putBoolean(ctx.getString(R.string.settings_show_fps), isEnabled);
        edit.commit();
    }

    public static void setAutoCount(Context ctx, boolean isAutoCount) {
        Editor edit = getEditor(ctx);
        edit.putBoolean(ctx.getString(R.string.settings_auto_count), isAutoCount);
        edit.commit();
    }

    public static boolean getAutoCount(Context ctx) {
        return getPrefs(ctx).getBoolean(ctx.getString(R.string.settings_auto_count), true);
    }

    public static Long getParticleNumber(Context ctx) {
        return getPrefs(ctx).getLong(ctx.getString(R.string.settings_particle_number), 0);
    }

    public static float getParticleSize(Context ctx) {
        return getPrefs(ctx).getFloat(ctx.getString(R.string.settings_particle_size), 3);
    }

    public static boolean getMotionBlur(Context ctx) {
        return getPrefs(ctx).getBoolean(ctx.getString(R.string.settings_particle_blur), true);
    }

    public static boolean getShowFps(Context ctx) {
        return getPrefs(ctx).getBoolean(ctx.getString(R.string.settings_show_fps), false);
    }

    public static int sizeToSeek(float size) {
        return (int) ((size - 1) * 10);
    }

    public static float seekToSize(int seek) {
        return 1.0f + (float) seek / 10.0f;
    }
}
