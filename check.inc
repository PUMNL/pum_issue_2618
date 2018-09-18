<?php

function pum_issue_2618_check_page() {
  pum_issue_2618_clean_webform_dir();

  $files = file_scan_directory('sites/default/files/webform', '/.*/');
  $totalFiles = count($files);
  $notFoundCount = 0;
  foreach($files as $file) {
    $fileFound = FALSE;
    $fids = db_select('file_managed', 'f')
      ->fields('f', ['fid'])
      ->condition('filename', $file->filename)
      ->execute();
    foreach ($fids as $fid) {
      $fileFound = TRUE;
      echo "File found (" . $file->filename . ", {$fid->fid}): ";
      $fileUsages = db_select('file_usage', 'f')->fields('f', array())->condition('fid', $fid->fid)->execute();
      foreach($fileUsages as $fileUsage) {
        echo " Used in module ".$fileUsage->module." ".$fileUsage->type." ".$fileUsage->id;
      }

      $webforms = db_query("SELECT sd.sid, sd.nid, sd.cid, wc.type, sd.data FROM webform_submitted_data sd INNER JOIN webform_component wc ON wc.nid = sd.nid AND wc.cid = sd.cid WHERE (wc.type = 'file' OR wc.type = 'multifile') AND sd.data LIKE '%{$fid->fid}%'");
      foreach ($webforms as $webform) {
        $webformFound = false;
        if ($webform->data === $fid->fid) {
          $webformFound = true;
        } else {
          $data = unserialize($webform->data);
          if (is_array($data) && in_array($fid, $data)) {
            $webformFound = true;
          }
        }

        if ($webformFound) {
          echo "<br>\r\n NID = " . $webform->nid . ", SID = " . $webform->sid . " Type = " . $webform->type . ", DATA: " . $webform->data;
        }
      }
    }
    if ($fileFound) {
      echo "<hr>\r\n";
    }
    if (!$fileFound) {
      $notFoundCount++;
    }
  }

  echo "<br>\r\nTotal files: ".$totalFiles.", not found: ".$notFoundCount;
  exit();
}