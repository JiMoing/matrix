/*
 * Tencent is pleased to support the open source community by making wechat-matrix available.
 * Copyright (C) 2018 THL A29 Limited, a Tencent company. All rights reserved.
 * Licensed under the BSD 3-Clause License (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package sample.tencent.matrix.fd;

import android.Manifest;
import android.app.Activity;
import android.content.DialogInterface;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AlertDialog;
import android.view.View;
import android.widget.Toast;

import com.tencent.matrix.Matrix;
import com.tencent.matrix.fdcanary.FDCanaryPlugin;
import com.tencent.matrix.iocanary.IOCanaryPlugin;
import com.tencent.matrix.iocanary.core.IOCanaryJniBridge;
import com.tencent.matrix.fdcanary.core.FDCanaryJniBridge;
import com.tencent.matrix.plugin.Plugin;
import com.tencent.matrix.util.MatrixLog;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import sample.tencent.matrix.R;
import sample.tencent.matrix.issue.IssueFilter;


/**
 * @author liyongjie
 * Created by liyongjie on 2017/6/9.
 */

public class TestFdActivity extends Activity {
    private static final String TAG = "Matrix.TestIoActivity";
    private static final int EXTERNAL_STORAGE_REQ_CODE = 0x1;

    private List<AlertDialog> dialogs = new ArrayList<>();

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.test_fd);
        IssueFilter.setCurrentFilter(IssueFilter.ISSUE_FD);
        requestPer();

        Plugin plugin = Matrix.with().getPluginByClass(FDCanaryPlugin.class);
        if (!plugin.isPluginStarted()) {
            MatrixLog.i(TAG, "plugin-fd start");
            plugin.start();
        }

    }

    private void requestPer() {
        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.WRITE_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {

            if (ActivityCompat.shouldShowRequestPermissionRationale(this,
                    Manifest.permission.WRITE_EXTERNAL_STORAGE)) {
                Toast.makeText(this, "please give me the permission", Toast.LENGTH_SHORT).show();
            } else {
                ActivityCompat.requestPermissions(this,
                        new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},
                        EXTERNAL_STORAGE_REQ_CODE);
            }
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        switch (requestCode) {
            case EXTERNAL_STORAGE_REQ_CODE:
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                }
                break;
        }
    }

    public void onClick(View v) {
        if (v.getId() == R.id.socket) {
            testSocket();
        } else if (v.getId() == R.id.dump) {
            FDCanaryJniBridge.dumpFdInfo();
        }
    }

    public void testSocket() {
        for (int i = 0; i < 10; i++) {
            //recount();
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            // 设置参数
            builder.setTitle("TEST")
                    .setMessage("TEST")
                    .setPositiveButton("CLOSE", new DialogInterface.OnClickListener() {// 积极

                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            for (AlertDialog a_dialog : dialogs) {
                                a_dialog.cancel();
                            }
                        }
                    }).setNegativeButton("DUMP", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    IOCanaryJniBridge.dumpFdInfo();
                }
            });
            AlertDialog dialog = builder.create();
            dialog.show();
            dialogs.add(dialog);
        }
    }


    public void toastStartTest(String val) {
        Toast.makeText(this, "starting io -> " + val + " test", Toast.LENGTH_SHORT).show();
    }
}
