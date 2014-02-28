
package com.valxp.particles;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;

public class PreferencesHelper {

    private static Editor getEditor(Context ctx) {
        return getPrefs(ctx).edit();
    }

    private static SharedPreferences getPrefs(Context ctx) {
        return ctx.getSharedPreferences(ctx.getString(R.string.settings_name), 0);
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

    public static Long getParticleNumber(Context ctx) {
        Long l  = Long.valueOf(0);
        try {
            l = getPrefs(ctx).getLong(ctx.getString(R.string.settings_particle_number), 0);
        } catch (Exception e) {
            try {
            l = (long) getPrefs(ctx).getInt(ctx.getString(R.string.settings_particle_number), 0);
            } catch (Exception e1) {
            }
        }
        return l;
    }

    public static float getParticleSize(Context ctx) {
        return getPrefs(ctx).getFloat(ctx.getString(R.string.settings_particle_size), 1);
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
        return 1.0f + (float)seek / 10.0f;
    }
}
