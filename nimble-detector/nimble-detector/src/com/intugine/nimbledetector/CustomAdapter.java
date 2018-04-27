
/* UNIMPORTANT
 * UNIMPORTANT
 * UNIMPORTANT
 * UNIMPORTANT
*/

package com.intugine.nimbledetector;

// package com.intugine.nimbledetector;

        import android.content.Context;
        import android.graphics.Typeface;
        import android.util.TypedValue;
        import android.view.View;
        import android.view.View.OnClickListener;
//import android.view.View.OnClickListener;
        import android.view.ViewGroup;
        import android.widget.LinearLayout;
        import android.widget.RelativeLayout;
        import android.widget.TextView;
        import android.widget.BaseAdapter;
        import android.widget.ImageView;
        import java.util.List;





class CustomAdapterView extends RelativeLayout {
    public CustomAdapterView(Context context, ListViewItem device)
    {
        super( context );
        // get/set ID
        setId(device.getDeviceID());
        // get image
        int padding = 5;
        int resImg = 0;
        String szStatus ="";
        if (device.getDeviceStatus()) {
            resImg = R.drawable.list_open;
            szStatus = "Open";
        } else {
            resImg = R.drawable.list;
            szStatus = "Not open";
        }



        // Configure holder layout
        setBackgroundResource(R.drawable.panel_item);
        //setOrientation(LinearLayout.HORIZONTAL);
        setPadding(padding, 0, padding, 0);
        //setGravity(Gravity.CENTER_VERTICAL);

        // LKEFT
        ImageView ivLogo = new ImageView(context);
        //    ivLogo.setId(100);
        ivLogo.setImageDrawable(context.getResources().getDrawable(resImg));
        //ivLogo.setBackgroundColor(Color.BLACK);
        RelativeLayout.LayoutParams lp_ivLogo = new RelativeLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        lp_ivLogo.addRule(RelativeLayout.CENTER_VERTICAL);
        lp_ivLogo.addRule(RelativeLayout.ALIGN_PARENT_LEFT);
        addView(ivLogo, lp_ivLogo);



        LinearLayout panelV = new LinearLayout(context);
        //panelV.setLayoutParams(new LinearLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));
        panelV.setOrientation(LinearLayout.VERTICAL);
        panelV.setPadding(padding,0,0,0);

        RelativeLayout.LayoutParams lp_panel = new RelativeLayout.LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT);
        lp_panel.addRule(RelativeLayout.CENTER_VERTICAL);
        lp_panel.addRule(RelativeLayout.RIGHT_OF, ivLogo.getId());
        //lp_panel.addRule(RelativeLayout.LEFT_OF, b.getId());
        addView(panelV, lp_panel);

        // row1
        TextView textName = new TextView( context );
        textName.setTextSize(TypedValue.COMPLEX_UNIT_SP, 12f);
        textName.setTypeface(Typeface.DEFAULT, Typeface.BOLD);

        textName.setText( device.getDeviceName());
        panelV.addView(textName);
        // row2
        TextView textAddress = new TextView( context );
        textAddress.setTextSize(TypedValue.COMPLEX_UNIT_SP, 10f);
        textAddress.setTypeface(Typeface.DEFAULT, Typeface.NORMAL);
        textAddress.setText(device.getDevicePath());
        panelV.addView(textAddress);
        // row3
        TextView textStatus = new TextView( context );
        textStatus.setTextSize(TypedValue.COMPLEX_UNIT_SP, 10f);
        textStatus.setTypeface(Typeface.DEFAULT, Typeface.BOLD);
        textStatus.setText(szStatus);
        panelV.addView(textStatus);



    }
}

public class CustomAdapter extends BaseAdapter {

    private Context context;
    private List<ListViewItem> deviceList;

    public CustomAdapter(Context context, List<ListViewItem> deviceList ) {
        this.context = context;
        this.deviceList = deviceList;
    }

    public int getCount() {
        return deviceList.size();
    }

    public Object getItem(int position) {
        return deviceList.get(position);
    }

    public long getItemId(int position) {
        return position;
    }

    public View getView(int position, View convertView, ViewGroup parent)
    {
        ListViewItem device = deviceList.get(position);
        View v = new CustomAdapterView(this.context, device );
        v.setOnClickListener((OnClickListener) context);
        return v;
    }
}
