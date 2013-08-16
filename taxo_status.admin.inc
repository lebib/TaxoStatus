<?php
/**
 * @file
 * Administrative page callbacks for the taxo_editor module.
 */


/**
 * FORM THEMING
 *
 * Implements hook_theme().
 */
function taxo_status_form_theme($existing, $type, $theme, $path) {
//  print('Debug2!');
	return array(
    'theme_taxo_status_form' => array(
      'render element' => 'element'
    ),
  );
}


function theme_taxo_status_form($elements){
	dprint_r($elements);
	$element = $elements['element'];
	drupal_add_tabledrag('form_id', 'order', 'sibling', 'rank-weight'); // for table dragging
	
	$header = array(
		'taxo_tid_active' => t('Activate'), 
		'step_type_taxo_tid' => t('Step type'),
		'weight' => t('Weight'),
	);
  $rows = array();
  foreach (element_children($element) as $key) {
    $row = array();
    
    $row['data'] = array();
    foreach ($header as $fieldname => $title) {
      $row['data'][] = drupal_render($element[$key][$fieldname]);
      $row['class'] = array('draggable'); // needed for table dragging
    }
    $rows[] = $row;
  }  
  return theme('table', array(
    'header' => $header, 
    'rows' => $rows,
    'attributes' => array('id' => 'form_id'), // needed for table dragging
  ));
}

/**
 * POPULATE DATAS
 * Configuration form
 *
 */
function taxo_status_form_get_content($type, $id = NULL){
	$select = array();
	if($type == 'vid'){
		$vocabs = taxonomy_vocabulary_get_names();
		foreach($vocabs as $vocab){
			$select[$vocab->vid] = $vocab->name;
		}
		return $select;
	}
	elseif($type == 'tid'){
		if(!isset($id)){
				$id = variable_get('taxo_status_vid');
		}
		$terms = taxonomy_get_tree($id);

//		foreach($terms as $term){
//			$select[$term->tid] = $term->name;
//		}
		$select = $terms;
		return $select;
	}
}

/**
 *
 * Initial form : vocabulary select with ajax loading for rest of form
 *
 */

function taxo_status_form($form, &$form_state){
	if(!isset($form_state['triggering_element']['#name'])){
	}
	else{
//		variable_set('taxo_status_use_vid', $form_state['triggering_element']['value']);
	}
		$form['taxo_status_vid'] = array(
			'#title' => t('Vocabulary to use'),
			'#type' => 'select',
			'#options' => taxo_status_form_get_content('vid'),
			'#required' => TRUE,
			'#default_value' => variable_get('taxo_status_vid', '0'),
			'#suffix' => '<div id="term_ids_warp"></div>',
		);
		$form['taxo_status_vid']['#ajax'] = array(
	  	'event' => 'change',
			'callback' => 'taxo_status_tids_form',
			'wrapper' => 'term_ids_warp',
			'method' => 'replace',
			'effect' => 'none',
		);
//	$form = array_merge($form,taxo_status_tids_form($form, $form_state, TRUE));
	if(variable_get('taxo_status_vid')){
	}
//	return system_settings_form($form);
//	dprint_r($form);
	return $form;
}
/**
 * Ajax loaded tid select and edit form.
 * Problem with numerical attribus
 *
 */
function taxo_status_tids_form($form, &$form_state = NULL, $display_all = FALSE){
// dprint_r($form_state);
	if($display_all){
		$selected_vid = variable_get('taxo_status_vid', '0');
	}
	else{
		$selected_vid = $form_state['values']['taxo_status_vid'];
	}
  $form_state['rebuild'] = TRUE;
	$form['taxo_status_tid'] = array(
		'#tree' => TRUE,
		'#theme' => 'theme_taxo_status_form',
	);
	$init = 0;
	foreach(taxo_status_form_get_content('tid', $selected_vid) as $term){
		$form['taxo_status_tid'][$term->tid]['taxo_tid_active'] = array(
			'#type' => 'checkbox',
			'#title' => $term->name,
			'#default_value' => $init,
		);
		$form['taxo_status_tid'][$term->tid]['taxo_tid_step_type'] = array(
			'#type' => 'radios',
			'#default_value' => '0',
			'#options' => array(
				0 => t('Multiple choice'),
				1 => t('Linear')
			)
		);
		$form['taxo_status_tid'][$term->tid]['weight'] = array(
			'#type' => 'textfield',
			'#default_value' => '0',
			'#size' => '3',
			'#attributes' => array(
				'class' => array(
					'rank-weight',
					)
				), // needed for table dragging
		);

	}
	  
  $form['taxo_status_tid']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  
  if($display_all){
  	$return = $form['taxo_status_tid'];
  }
  else{
  	$return = drupal_render($form['taxo_status_tid']);
  }
	return $return;
}

function taxo_status_form_submit($form, &$form_state) {
  uasort($form_state['values']['taxo_status_tid'], 'drupal_sort_weight');
  dsm($form_state['values']['taxo_status_tid']);
}
