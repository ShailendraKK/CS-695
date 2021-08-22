#include "vm_lib.c"

#include <gtk/gtk.h>
 int max_cpu,min_cpu,vm_wait;
/*enum
{
  VM_NAME = 0,
  STATUS,
  IP_ADDRESS,
  CPU_UTILZATION,
  NUM_COLS
} ;*/
//GtkTreeModel        *model;
 //GtkWidget           *view;




// static GtkWidget *
// create_view_and_model (void)
// {
//   GtkCellRenderer     *renderer;


//   view = gtk_tree_view_new ();

//   /* --- Column #1 --- */

//   renderer = gtk_cell_renderer_text_new ();
//   gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
//                                                -1,      
//                                                "VM Name",  
//                                                renderer,
//                                                "text", VM_NAME,
//                                                NULL);

//   /* --- Column #2 --- */

//   renderer = gtk_cell_renderer_text_new ();
//   gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
//                                                -1,      
//                                                "Status",  
//                                                renderer,
//                                                "text", STATUS,
//                                                NULL);

//     renderer = gtk_cell_drenderer_text_new ();
//   gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
//                                                -1,      
//                                                "IP Address",  
//                                                renderer,
//                                                "text", IP_ADDRESS,
//                                                NULL);
//       renderer = gtk_cell_renderer_text_new ();
//   gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
//                                                -1,      
//                                                "CPU Utilization",  
//                                                renderer,
//                                                "text", CPU_UTILZATION,
//                                                NULL);

//  // model = create_and_fill_model ();

//   //gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);

//   /* The tree view has acquired its own reference to the
//    *  model, so we can drop ours. That way the model will
//    *  be freed automatically when the tree view is destroyed */

//   //g_object_unref (model);

//   return view;
// }

void initialize_grid()
{

  GtkWidget *grid;
  GtkWidget *button;
  GtkWidget *label_tmp;
  GtkWidget *view;
 
  //gtk_init (&argc, &argv);
  
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
   
  gtk_window_set_title (GTK_WINDOW (window), "VM Manager");
  gtk_window_set_default_size (GTK_WINDOW (window), 400,400 );
   
  g_signal_connect (window, "delete_event", gtk_main_quit, NULL); /* dirty */
  /* create a new window, and set its title */
//  window = gtk_application_window_new (app);
 // gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  /* Here we construct the container that is going pack our buttons */
  grid = gtk_grid_new ();
  gtk_grid_set_column_spacing(GTK_GRID(grid),20);
  /* Pack the container in the window */
  gtk_container_add (GTK_CONTAINER (window), grid);


  label_tmp = gtk_label_new("Server name");
  gtk_grid_attach (GTK_GRID (grid), label_tmp, 0, 0, 1, 1);
  
    label_tmp = gtk_label_new("Server IP");
  gtk_grid_attach (GTK_GRID (grid), label_tmp, 1, 0, 1, 1);

    label_tmp = gtk_label_new("Server status");
  gtk_grid_attach (GTK_GRID (grid), label_tmp, 2, 0, 1, 1);

     label_tmp = gtk_label_new("CPU utilization");
  gtk_grid_attach (GTK_GRID (grid), label_tmp, 3, 0, 1, 1);

    label_tmp = gtk_label_new("Start VM");
  gtk_grid_attach (GTK_GRID (grid), label_tmp, 4, 0, 1, 1);

    label_tmp = gtk_label_new("Stop VM");
  gtk_grid_attach (GTK_GRID (grid), label_tmp, 5, 0, 1, 1);
 num_total_vm_global=virConnectListAllDomains(conn, &dom_ptr_global, VIR_CONNECT_LIST_DOMAINS_PERSISTENT);

  for(int i=0;i<num_total_vm_global;i++){
       for(int j=0;j<4;j++){
         label[i][j]=gtk_label_new("");
           gtk_grid_attach (GTK_GRID (grid), label[i][j], j, i+1, 1, 1);

       }

  btn_start[i] = gtk_button_new_with_label ("Start");
  g_signal_connect (btn_start[i], "clicked", G_CALLBACK (start_resp_vm), NULL);

  /* Place the first button in the grid cell (0, 0), and make it fill
   * just 1 cell horizontally and vertically (ie no spanning)
   */
  gtk_grid_attach (GTK_GRID (grid), btn_start[i],4 , i+1, 1, 1);


   btn_stop[i] = gtk_button_new_with_label ("Stop");
  g_signal_connect (btn_stop[i], "clicked", G_CALLBACK (stop_resp_vm), NULL);

  /* Place the Quit button in the grid cell (0, 1), and make it
   * span 2 columns.
   */
  gtk_grid_attach (GTK_GRID (grid), btn_stop[i], 5, i+1, 1, 1);

  }
  
  /* Now that we are done packing our widgets, we show them all
   * in one go, by calling gtk_widget_show_all() on the window.
   * This call recursively calls gtk_widget_show() on all widgets
   * that are contained in the window, directly or indirectly.
   */
  gtk_widget_show_all (window);

}
int
main (int argc, char **argv)
{

  max_cpu=50;
  min_cpu=20;
  vm_wait=40;
  if(argc > 1){
    char *token;
    if(argc< 4){
      printf("Usage: ./gui -max_cpu=XXX -min_cpu=YYY -vm_wait=ZZZ");
      return 0;
    }
    else{
       for(int i=1;i<argc;i++){
          char input[strlen(argv[i])];
          strcpy(input, argv[i]);
          token = strtok(input, "=");
          token = strtok(NULL, "=");
          switch (i) {
          case 1:
            max_cpu = atoi(token);
            break;
          case 2:
            min_cpu = atoi(token);
            break;
          case 3:
            vm_wait = atoi(token);
            break;
          }
          
       }
    }
  }
  
  GtkWidget *view;
 
  gtk_init (&argc, &argv);
  
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
   
  gtk_window_set_title (GTK_WINDOW (window), "VM Manager");
  gtk_window_set_default_size (GTK_WINDOW (window), 400,400 );
   
  g_signal_connect (window, "delete_event", gtk_main_quit, NULL); /* dirty */
    
 // view = create_view_and_model ();
   
 // gtk_container_add (GTK_CONTAINER (window), view);
       conn = virConnectOpen("qemu:///system");
    if (conn == NULL) {
        fprintf(stderr, "Failed to open connection to qemu:///system\n");
        return 1;
    }
  
   initialize_grid();
  gtk_widget_show_all (window);
 //GtkApplication *app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  //g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
 // initialize_grid();
  pthread_t pid_t;
  pthread_create(&pid_t,NULL,&initialize,NULL);
  
  gtk_main ();
   fclose(fopen("server_ip", "w"));
  return 0;
}
