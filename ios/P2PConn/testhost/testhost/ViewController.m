//
//  ViewController.m
//  testhost
//
//  Created by redcdn on 18/2/11.
//  Copyright © 2018年 redcdn. All rights reserved.
//

#import "ViewController.h"
#import "MeetingHostIos.hpp"


@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    StartHost();
    
    sleep(2);
    
    SendShare();

    sleep(2);
    
    SendData();
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)OnRegister:(id)sender {
    SendShare();
}

- (void)OSendData:(id)sender {
    SendData();
}

@end
